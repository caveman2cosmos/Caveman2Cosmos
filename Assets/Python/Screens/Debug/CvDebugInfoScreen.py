## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvScreenEnums
import CvGFCScreen

# globals
gc = CyGlobalContext()

list_Widget = []
g_iLastScreen = 0

class CvDebugInfoScreen(CvGFCScreen.CvGFCScreen):
	'Debug Information Screen'

	def __init__(self):
		self.setScreenValues("DebugInfoScreen", CvScreenEnums.DEBUG_INFO_SCREEN)

		self.DebugInfoMap = {
			 0 : {'DESC': "Team Relations",		'DATA': self.drawTeamRelations},
			 1 : {'DESC': "Player Attitudes",	'DATA': self.drawPlayerAttitudes},
			 2 : {'DESC': "Civic Info",			'DATA': self.drawPlayerCivics},
			 3 : {'DESC': "Civ Info",			'DATA': self.drawPlayerOverviewInfo},
			 4 : {'DESC': "AI Types",			'DATA': self.drawPlayerUnitAITypes},
			 5 : {'DESC': "Available Bonuses",	'DATA': self.drawPlayerAvailableResources},
			 6 : {'DESC': "Map Details",		'DATA': self.generateBonusDistributionList},
		}
		self.CivInfoItems = [
			"Power", "Assets", "Gold Reserve", "Gold Commerce", "Research Rate",
			"Total Maintenance", "Civic Upkeep", "Unit Cost", "Unit Supply",
			"# Cities", "# Units", "# Citizens", "# Plots", "Current Tech"
		]

		self.WIDGET_ID = "DebugInfoScreenWidget"
		self.nWidgetCount = 0

		self.szDebugInfoPulldownID = "DebugInfoScreenSelectType"
		self.bTeam = false
		self.bAttitude = false
		self.iNumPlayers = -1
		self.iNumTeams = -1

		self.W_CHART = 800
		self.H_CHART = 400

		self.TITLE_TEXT_X = 502
		self.TITLE_TEXT_Y = 8
		self.TITLE_TEXT_Z = -2.5

		self.EXIT_TEXT_X = 502
		self.EXIT_TEXT_Y = 12
		self.EXIT_TEXT_Z = -2.6

		self.DROPDOWN_INFOTYPE_X = 405
		self.DROPDOWN_INFOTYPE_Y = 47

		self.TABLE_START_X = 30
		self.TABLE_START_Y = 70

	def interfaceScreen(self):
		' Create the screen '
		global g_iLastScreen
		screen = self.getScreen()
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.addDDSGFC("DebugInfoScreen", CyArtFileMgr().getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, 1024, 768, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.setExitText("EXIT SCREEN", 1<<1, self.EXIT_TEXT_X, self.EXIT_TEXT_Y, self.EXIT_TEXT_Z, FontTypes.TITLE_FONT)

		# Header...
		screen.setText(
			"DebugInfoTitleHeader", "Background", u"DEBUG INFO",
			1<<2, self.TITLE_TEXT_X, self.TITLE_TEXT_Y, self.TITLE_TEXT_Z,
			FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1
		)
		# Create Drop Down so the user can select the screen they want
		screen.addDropDownBoxGFC(self.szDebugInfoPulldownID,
								self.DROPDOWN_INFOTYPE_X, self.DROPDOWN_INFOTYPE_Y, 300,
								WidgetTypes.WIDGET_GENERAL, -1, -1,FontTypes.GAME_FONT )

		# adds the screen names to the drop down using the DebugInfoMap - always selects the last one that was selected
		bSelected = false

		for i in range(len(self.DebugInfoMap)):
			if i == g_iLastScreen:
				bSelected = true

			strInfoName = self.DebugInfoMap[i].get('DESC')

			screen.addPullDownString( self.szDebugInfoPulldownID, strInfoName, i, -1, bSelected )
			bSelected = false
		#draw the contents with the last selected screen
		self.drawContents(g_iLastScreen)

	def drawContents(self, iInfoType = 0):
		'Based on dropdown selection the screen will draw the appropriate info screen'
		global g_iLastScreen
		g_iLastScreen = iInfoType

		entry = self.DebugInfoMap.get(iInfoType)
		entry.get('DATA')()

	def drawTeamRelations(self):
		' draws a table showing the relations of teams '
		d_TableData = {}
		d_TableData[0] = self.setTeams()

		strBuffer = ""
		list_loopRowData = []

		for x in range(self.iNumTeams):
			strBuffer = gc.getTeam(x).getName()
			list_loopRowData.append(strBuffer)
			strBuffer = ""

			for y in range(self.iNumTeams):
				pTeamX = gc.getTeam(x)
				#pTeamY = gc.getTeam(y)

				# if the same team
				if x != y:

					# if teams have met
					if pTeamX.isHasMet(y):

						# if at peace
						if not pTeamX.isAtWarWith(y):

							# if defensive pact
							if pTeamX.isDefensivePact(y):
								strBuffer = "DefensivePact"
							# elif open borders
							elif pTeamX.isOpenBorders(y):
								strBuffer = "OpenBorders"
							# else peace
							else: strBuffer = "Peace"

						# else indicate at War
						else:
							strBuffer = "War"

					# else they have not met
					else:
						strBuffer = "NotMet"

				# else use ally icon
				else:
					strBuffer = "Team"

				list_loopRowData.append(strBuffer)

			d_TableData[x+1] = list_loopRowData
			list_loopRowData = []

		# add data to table
		self.setTableData(d_TableData)

		return 0

	def drawPlayerAttitudes(self):
		' draws a table showing the attitudes between players '
		self.bAttitude = True
		d_TableData = {}
		d_TableData[0] = self.setPlayers()

		strBuffer = ""
		list_loopRowData = []

		for x in range(self.iNumPlayers):

			strBuffer = gc.getPlayer(x).getName()
			list_loopRowData.append(strBuffer)
			strBuffer = ""

			for y in range(self.iNumPlayers):
				pPlayerX = gc.getPlayer(x)
				pPlayerY = gc.getPlayer(y)

				pTeamX = gc.getTeam(pPlayerX.getTeam())
				#pTeamY = gc.getTeam(pPlayerY.getTeam())

				# if the same Player
				if ( not x == y ):

					# if teams have met
					if pTeamX.isHasMet(y):

						iPlayerXAttitudeTowardPlayerY = pPlayerX.AI_getAttitude(y)

						strBuffer = "%s" %(gc.getAttitudeInfo(iPlayerXAttitudeTowardPlayerY).getDescription(),)
						list_loopRowData.append(strBuffer)
						strBuffer = ""
						continue # prevents the value from getting overwritten by a font icon

					# else they have not met
					else:
						strBuffer = "NotMet"

				# else use ally icon
				else:
					strBuffer = "Ally"

				list_loopRowData.append(strBuffer)
				strBuffer = ""

			d_TableData[x+1] = list_loopRowData
			list_loopRowData = []

		# add data to table
		self.setTableData(d_TableData)

		return 0

	def drawPlayerCivics(self):
		d_TableData = {}
		strName = ""
		list_loopRowData = []

		d_TableData[0] = self.setPlayers("Civics")

		for i in range( gc.getNumCivicOptionInfos() ):
			strName = gc.getCivicOptionInfo(i).getDescription()
			list_loopRowData.append(strName)

			for j in range( self.iNumPlayers ):
				strName = gc.getCivicInfo( int(gc.getPlayer(j).getCivics(i)) ).getDescription()
				list_loopRowData.append(strName)

			d_TableData[i+1] = list_loopRowData
			list_loopRowData = []

		self.setTableData(d_TableData)

		return 0

	def drawPlayerOverviewInfo(self):
		d_TableData = {}
		strName = ""
		list_loopRowData = []

		d_TableData[0] = self.setPlayers("Civ Info")

		for i in range(len(self.CivInfoItems)):
			strName = self.CivInfoItems[i]
			list_loopRowData.append(strName)

			for j in range(self.iNumPlayers):
				pPlayer = gc.getPlayer(j)

				iTech = pPlayer.getCurrentResearch()
				if iTech > -1:
					szCurrentTech = gc.getTechInfo(iTech).getDescription()
				else: szCurrentTech = "No Research"

				CivInfoFnx = [
					pPlayer.getPower(), pPlayer.getAssets(), pPlayer.getGold(),
					pPlayer.getCommerceRate(CommerceTypes.COMMERCE_GOLD),
					pPlayer.calculateResearchRate(TechTypes.NO_TECH),
					pPlayer.getTotalMaintenance(), pPlayer.getCivicUpkeep(False),
					pPlayer.calculateUnitCost(), pPlayer.calculateUnitSupply(),
					pPlayer.getNumCities(), pPlayer.getNumUnits(),
					pPlayer.getTotalPopulation(), pPlayer.getTotalLand(),
					szCurrentTech
				]
				strName = str(CivInfoFnx[i])
				list_loopRowData.append(strName)

			d_TableData[i+1] = list_loopRowData
			list_loopRowData = []

		self.setTableData(d_TableData)

		return 0

	def drawPlayerUnitAITypes(self):
		self.TITLE_X = 200
		self.COLUMN_X = 75

		d_TableData = {}
		strName = ""
		list_loopRowData = []

		d_TableData[0] = self.setPlayers("Unit AI")

		for i in range( UnitAITypes.NUM_UNITAI_TYPES ):
			strName = gc.getUnitAIInfo(i).getDescription()
			strName.strip("UNITAI_")
			list_loopRowData.append(strName)

			for j in range( self.iNumPlayers ):
				strName = "%s" %( int(gc.getPlayer(j).AI_getNumAIUnits(i)), )
				list_loopRowData.append(strName)

			d_TableData[i+1] = list_loopRowData
			list_loopRowData = []

		self.setTableData(d_TableData)

		return 0

	def drawPlayerAvailableResources(self):
		' Draws a table showing how many of each resource the player can trade '
		d_TableData = {}
		list_loopRowData = []

		d_TableData[0] = self.setPlayers("Available Bonuses")

		for i in range( gc.getNumBonusInfos() ):
			list_loopRowData.append(gc.getBonusInfo(i).getDescription())

			for j in range( self.iNumPlayers ):
				list_loopRowData.append( int(gc.getPlayer(j).getNumAvailableBonuses(i)) )
			d_TableData[i+1] = list_loopRowData
			list_loopRowData = []

		self.setTableData(d_TableData)

		return 0

	def generateBonusDistributionList(self):
		"Displays Bonus Advisor Popup"
		d_TableData = {}

		map = CyMap()

		areas = map.areas()
		area_sizes = [(area.getNumTiles(), area) for area in areas]
		area_sizes.sort() # sort by size -- biggest area last
		area_sizes.reverse() # biggest area first
		areas = [area for (area_size, area) in area_sizes]

		bonus_infos = [gc.getBonusInfo(i) for i in range(gc.getNumBonusInfos())]
		title_list = [u"BonusDistro"] + [u"Area"] + [u"Size"] + [u"Starting Plots"] + [u"Unique Bonus Types"] + [u"Total Bonuses"]

		bonusList = ""
		for bonus in bonus_infos:
			bonusList += bonus.getDescription()[0]
		title_list += bonusList

		d_TableData[0] = title_list

		total = 0
		loopRowData = []
		for i in range(len(areas)):
			area = areas[i]
			area_string = "%d" % area.getID()
			if area.isWater():
				area_string = "w" + area_string

			loopRowData.append(area_string)
			loopRowData.append("%d" % area.getNumTiles())
			start_plots = area.getNumStartingPlots()
			if start_plots > 0:
				loopRowData.append("%d" % start_plots)
			else:
				loopRowData.append("")
			num_unique = area.countNumUniqueBonusTypes()
			if num_unique > 0:
				loopRowData.append("%d" % num_unique)
			else:
				loopRowData.append("")

			total_in_area = area.getNumTotalBonuses()
			total += total_in_area
			loopRowData.append("%d" % total_in_area)

			for j in range(len(bonus_infos)):
				quantity = area.getNumBonuses(j)
				if quantity > 0:
					loopRowData.append("%d" % quantity)
				else:
					loopRowData.append("")
			d_TableData[i+1] = loopRowData
			loopRowData = []

		# Add last row of totals:
		#last_row = len(areas) + 1
		loopRowData.append("Total")
		loopRowData.append(map.numPlots())
		loopRowData.append("")
		loopRowData.append("")
		loopRowData.append("%d" % total)
		for j in range(len(bonus_infos)):
			quantity = map.getNumBonuses(j)
			if quantity > 0:
				loopRowData.append("%d" % quantity)
			else:
				loopRowData.append("-")
		iLastRow = len(d_TableData)
		d_TableData[iLastRow] = loopRowData

		#print d_TableData
		self.setTableData(d_TableData)

	def setTeams(self, strTitle = "Teams"):
		self.bTeam = true
		self.iNumTeams = gc.getGame().countCivTeamsEverAlive()
		return self.buildTeamTitleData(strTitle)

	def setPlayers(self, strTitle = "Players"):
		self.iNumPlayers = gc.getGame().countCivPlayersEverAlive()
		return self.buildPlayerTitleData(strTitle)

	def setTableData(self, d_TableData):
		' adds the Table Data to the screen '
		screen = self.getScreen()

		numColumns = len(d_TableData[0])

		#print "NumColumns = %d" %numColumns
		#print d_TableData

		#scrollPanel = self.getNextWidgetName()
		#screen.addScrollPanel(scrollPanel, u"", 0, 100, 1000,580, PanelStyles.PANEL_STYLE_DEFAULT)

		szTableName = self.getNextWidgetName()
		#screen.attachTableControlGFC(scrollPanel, szTableName, numColumns, True, True, 32,32,TableStyles.TABLE_STYLE_STANDARD)
		screen.addTableControlGFC(szTableName, numColumns, 0, 100, 1000, 580, True, True, 32,32,TableStyles.TABLE_STYLE_STANDARD)

		columnHeaders = d_TableData[0]

		for i in range(len(columnHeaders)):
			columnWidth = 70
			if i == 0:
				columnWidth = 150
			screen.setTableColumnHeader(szTableName, i, columnHeaders[i], columnWidth)
		i=0
		for i in range(len(d_TableData)-1):
			screen.appendTableRow(szTableName)
		i=0
		for i in range(len(d_TableData)-1):
			rowData = d_TableData[i+1]
			for j in range(len(rowData)):
				#print rowData[j]
				screen.setTableText(szTableName, j, i, u"%s" %rowData[j], "", WidgetTypes.WIDGET_GENERAL,-1,-1, 1<<0)

	def buildTeamTitleData(self, strTitle):
		' return a '
		title_list = []

		for i in range( self.iNumTeams ):
			strName = gc.getTeam(i).getName()

			title_list.append( strName )

		title_list.insert( 0,strTitle )

		return title_list

	def buildPlayerTitleData(self, strTitle):
		' return a '
		title_list = []

		for i in range( self.iNumPlayers ):
			strName = gc.getPlayer(i).getName()
			title_list.append( strName )

		title_list.insert( 0,strTitle )

		return title_list

	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllWidgets(self, iNumPermanentWidgets = 0):
		screen = self.getScreen()
		i = self.nWidgetCount - 1
		while (i >= iNumPermanentWidgets):
			self.nWidgetCount = i
			screen.deleteWidget(self.getNextWidgetName())
			i -= 1

		self.nWidgetCount = iNumPermanentWidgets
		self.yMessage = 5

	def InfoType (self, inputClass):
		screen = self.getScreen()
		'called when the player activates the drop down box'
		#print inputClass.getNotifyCode()

		if ( inputClass.getNotifyCode() == int(NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED) ) :
			#return
			#szPulldownID = str(inputClass.getID())
			# Get the ID that was selected...
			iSelectedID = screen.getSelectedPullDownID( self.szDebugInfoPulldownID )
			self.deleteAllWidgets()
			self.drawContents(iSelectedID)

	def handleInput (self, inputClass):
		'Calls function mapped in FinanceAdvisorInputMap'
		functionName = inputClass.getFunctionName()
		print functionName
		if ( functionName == self.szDebugInfoPulldownID ):
			self.InfoType(inputClass)
		return 0

	def update(self, fDelta):
		return
