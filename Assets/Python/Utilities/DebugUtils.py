
from CvPythonExtensions import *
import Popup as PyPopup

GC = CyGlobalContext()
bDebugMode = False

def toggleDebugMode():
	global bDebugMode
	bDebugMode = not bDebugMode
	CyInterface().addImmediateMessage("Python Debug Mode: %s" % bDebugMode, "AS2D_ERROR")

def isAnyDebugMode():
	return bDebugMode or GC.getGame().isDebugMode()

class DebugUtils:
	def __init__(self):
		self.iLastUnitPicker = -1

	def resetUnitMovement(self):
		for iPlayer in xrange(GC.getMAX_PLAYERS()):
			for CyUnit in GC.getPlayer(iPlayer).units():
				CyUnit.setMoves(0)

	def allBonuses(self):
		iNBonuses = GC.getNumBonusInfos()
		map = GC.getMap()
		if iNBonuses < map.getGridWidth() * map.getGridHeight():
			for x in xrange(map.getGridWidth()):
				for y in xrange((iNBonuses/map.getGridWidth())+1):
					map.plot(x,y).setBonusType((x + y * map.getGridWidth())%iNBonuses)

	def allImprovements(self):
		iNImprovements = GC.getNumImprovementInfos()
		map = GC.getMap()
		if (iNImprovements < map.getGridWidth() * map.getGridHeight()):
			for x in xrange(map.getGridWidth()):
				for y in xrange((iNImprovements/map.getGridWidth())+1):
					map.plot(x,y).setImprovementType((x + y * map.getGridWidth())%iNImprovements)


################ TRIGGERED EVENTS ################

	# Event 1050
	def initUnitPicker(self, px, py):
		pPlot = GC.getMap().plot(px,py)
		popup = PyPopup.PyPopup(1050, EventContextTypes.EVENTCONTEXT_ALL)
		popup.setSize(400,600)
		popup.setPosition(600,25)
		popup.setUserData((px,py))
		popup.setHeaderString("Python Debug Tools: Object Placer")

		# Pulldown0 - Player Selection
		iNumUnits = GC.getNumUnitInfos()	# get total # of units from Game
		iOwner = pPlot.getOwner()
		popup.createPythonPullDown("Choose a Player")
		if iOwner < 0:
			iOwner = GC.getGame().getActivePlayer()
		if iOwner > -1:
			popup.addPullDownString(GC.getPlayer(iOwner).getName(), iOwner)
		for i in xrange(GC.getMAX_PLAYERS()):
			if i == iOwner: continue
			CyPlayer = GC.getPlayer(i)
			if CyPlayer.isAlive():
				popup.addPullDownString(CyPlayer.getName(), i)

		popup.addSeparator()

		popup.createPythonListBox("")
		popup.addListBoxString('Nothing', iNumUnits + 1)   # for clean exit
		popup.addSeparator()

		if self.iLastUnitPicker > -1:
			if self.iLastUnitPicker == iNumUnits + 1:
				szTxt = 'Nothing'
			elif self.iLastUnitPicker == iNumUnits:
				szTxt = 'City'
			else:
				szTxt = GC.getUnitInfo(self.iLastUnitPicker).getDescription()
				popup.addListBoxString(szTxt, self.iLastUnitPicker)

		popup.addListBoxString('City', iNumUnits) # list City first

		# sort units alphabetically
		unitsList=[(0,0)]*iNumUnits
		for j in xrange(iNumUnits):
			unitsList[j] = (GC.getUnitInfo(j).getDescription(), j)
		unitsList.sort()

		for j in xrange(iNumUnits):
			popup.addListBoxString(unitsList[j][0], unitsList[j][1])

		# EditBox0 - Customize how many units to build
		#popup.setBodyString("How many objects?")
		popup.createPythonEditBox("1", "This allows you to create multiple units.")

		# Launch Popup
		#popup.setSize(400, 600)
		popup.launch()
		return 0

	def applyUnitPicker(self, iPlayer, userData, popupReturn):
		iX, iY = userData

		if GC.getMap().plot(iX, iY).isNone():
			return 0

		# UNIT DEBUG MENU
		iPlayer = popupReturn.getSelectedPullDownValue(0)
		iObject = popupReturn.getSelectedListBoxValue(0)

		CyPlayer = GC.getPlayer(iPlayer)
		if CyPlayer is None:
			return -1 # Error

		iNumUnits = GC.getNumUnitInfos()

		if iObject < iNumUnits + 1:
			self.iLastUnitPicker = iObject

			if iObject == iNumUnits: # City"
				CyPlayer.initCity(iX, iY)
			else:
				iSpawnNum = int(popupReturn.getEditBoxString(0))
				while iSpawnNum > 0:
					CyPlayer.initUnit(iObject, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
					iSpawnNum -= 1
		else:
			self.iLastUnitPicker = -1

		return 0

debugUtils = DebugUtils()


# Event 1000
def initEffectViewer(px, py):
	popup = PyPopup.PyPopup(1000, EventContextTypes.EVENTCONTEXT_SELF)
	popup.setSize(550,300)
	popup.setUserData((px,py))
	popup.setHeaderString("Python Debug Tools: Object Placer")
	# Pulldown0 - Player Selection
	popup.createPythonPullDown("Choose an Effect")
	for i in xrange(GC.getNumEffectInfos()):
		popup.addPullDownString(GC.getEffectInfo(i).getType(), i)

	popup.createPythonEditBox("Default", "Modify the scale of the effect")
	popup.createPythonEditBox("Default", "Modify the update rate")

	# Launch Popup
	popup.launch()
	return 0

def applyEffectViewer(iPlayer, userData, popupReturn):
	px, py = userData
	CyPlot = GC.getMap().plot(px, py)
	if not CyPlot.isNone():
		CyEngine().triggerEffect(popupReturn.getSelectedPullDownValue(0), CyPlot.getPoint())


# Event 1001
def initWonderMovie():
	popup = PyPopup.PyPopup(1001, EventContextTypes.EVENTCONTEXT_ALL)
	popup.setHeaderString("Wonder Movie")
	popup.createPullDown()
	for i in xrange(GC.getNumBuildingInfos()):
		szMovieFile = GC.getBuildingInfo(i).getMovie()
		if szMovieFile:
			popup.addPullDownString(GC.getBuildingInfo(i).getDescription(), i)

	for i in xrange(GC.getNumProjectInfos()):
		szArtDef = GC.getProjectInfo(i).getMovieArtDef()
		if szArtDef:
			szMovieFile = CyArtFileMgr().getMovieArtInfo(szArtDef).getPath()
			if szMovieFile:
				popup.addPullDownString(GC.getProjectInfo(i).getDescription(), GC.getNumBuildingInfos() + i)

	popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

def applyWonderMovie(iPlayer, userData, popupReturn):

	wonderID = popupReturn.getSelectedPullDownValue(0)

	popupInfo = CyPopupInfo()
	popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
	popupInfo.setData2(-1)
	popupInfo.setText(u"showWonderMovie")

	if wonderID < GC.getNumBuildingInfos():
		popupInfo.setData3(0)
		popupInfo.setData1(wonderID)
	else:
		popupInfo.setData3(2)
		popupInfo.setData1(wonderID - GC.getNumBuildingInfos())

	popupInfo.addPopup(0)


# Event 1002
def initTechsCheat():
	popup = PyPopup.PyPopup(1002, EventContextTypes.EVENTCONTEXT_ALL)
	popup.setHeaderString("Tech & Gold Cheat!")
	popup.createPullDown()
	popup.addPullDownString("All", GC.getMAX_PLAYERS())
	for i in xrange(GC.getMAX_PLAYERS()):
		if (GC.getPlayer(i).isAlive()):
			popup.addPullDownString(GC.getPlayer(i).getName(), i)
	popup.setBodyString("Modify Player " + unichr(8500))
	popup.createPythonEditBox("0", "Integer value (positive or negative)")

	for i in xrange(GC.getNumEraInfos()):
		popup.addButton(GC.getEraInfo(i).getDescription())

	popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)

def applyTechCheat(iPlayer, userData, popupReturn):

	iPlayer = popupReturn.getSelectedPullDownValue(0)
	iPlayers = 0
	if iPlayer == GC.getMAX_PC_PLAYERS():
		player = []
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive():
				player.append(CyPlayerX)
				iPlayers += 1

	else: player = GC.getPlayer(iPlayer)

	try:
		goldChange = int(popupReturn.getEditBoxString(0))
	except:
		goldChange = 0

	if goldChange:
		if iPlayers:
			for i in xrange(iPlayers):
				player[i].changeGold(goldChange)

		else: player.changeGold(goldChange)

	era = popupReturn.getButtonClicked()
	for iTech in xrange(GC.getNumTechInfos()):

		if GC.getTechInfo(iTech).getiEra() == era:
			if iPlayers:
				for j in xrange(iPlayers):
					player[j].setHasTech(iTech)
			else:
				player.setHasTech(iTech)

# Event 1003
def initEditCity(px, py):
	local = CyTranslator()
	city = CyMap().plot(px,py).getPlotCity()
	iOwner = city.getOwner()
	userData = (iOwner, city.getID())

	# create popup
	popup = PyPopup.PyPopup(1003, EventContextTypes.EVENTCONTEXT_ALL)
	popup.setSize(600, 600)
	popup.setPosition(16, 64)
	popup.setUserData(userData)
	popup.setHeaderString(local.getText("TXT_KEY_WB_HEADER_CITY_EDIT", ()))
	# City Name - Editbox 0
	popup.createEditBox(city.getName(), 0)
	# Population - Editbox 1
	popup.createEditBox('0', 1)
	popup.setBodyString(local.getText("TXT_KEY_WB_CITY_EDIT_POP", ()))
	# Culture - Editbox 2
	popup.createEditBox('0', 2)
	popup.setBodyString(local.getText("TXT_KEY_WB_CITY_EDIT_CULTURE", (city.getCulture(iOwner),)))
	# Buildings - Listboxes Group
	popup.createListBox(0)
	iNumBuildings = GC.getNumBuildingInfos()
	lBuildings = []
	for i in range(iNumBuildings):
		# ('Library', iIndex)
		lBuildings.append((GC.getBuildingInfo(i).getDescription(), i))
	lBuildings.sort()

	popup.addListBoxString(local.getText("TXT_KEY_WB_CITY_NOTHING", ()), -1)
	popup.addListBoxString(local.getText("TXT_KEY_WB_CITY_ALL", ()), iNumBuildings) #for adding/removing every building
	for i in range(len(lBuildings)):
		entry = lBuildings[i]
		popup.addListBoxString(entry[0], entry[1])

	# Add/Remove - Pulldown Group 0
	popup.createPullDown(0)
	popup.addPullDownString(local.getText("TXT_KEY_WB_CITY_ADD", ()), 1)
	popup.addPullDownString(local.getText("TXT_KEY_WB_CITY_NOCHANGE", ()), 0) #for clean exit
	popup.addPullDownString(local.getText("TXT_KEY_WB_CITY_REMOVE", ()), 2)
	popup.setBodyString(local.getText("TXT_KEY_WB_CITY_ADD_REMOVE", ()))

	# Launch Popup
	popup.launch()

def applyEditCity(iPlayer, userData, popupReturn):
	iOwner, iID = userData
	city = GC.getPlayer(iOwner).getCity(iID)
	if city is None:
		return 0

	# Name
	szName = str(popupReturn.getEditBoxString(0))
	if szName != city.getName():
		city.setName(szName, False)

	# Population
	iValue = int(popupReturn.getEditBoxString(1))
	if iValue:
		city.changePopulation(iValue)

	iValue = int(popupReturn.getEditBoxString(2))
	if iValue:
		city.setCulture(iOwner, iValue, True)

	# Buildings
	iNumBuildings = GC.getNumBuildingInfos()
	iBuilding = popupReturn.getSelectedListBoxValue(0)
	if iBuilding > -1 and popupReturn.getSelectedPullDownValue(0) > 0:
		if iBuilding == iNumBuildings + 1:
			return 0
		# Toggles buildings in cities
		add = popupReturn.getSelectedPullDownValue(0) == 1
		if iBuilding != -1:
			if iBuilding == iNumBuildings:
				for i in range(iBuilding):
					city.setNumRealBuilding(i, add)
			else:
				city.setNumRealBuilding(iBuilding, add)


def putOneOfEveryUnit():
	MAP = GC.getMap()

	x = y = 0
	dx = MAP.getGridWidth()
	dy = MAP.getGridHeight()

	waterPlots = []
	landPlots = []
	iLand = 0
	iWater = 0
	while y < dy:

		CyPlot = MAP.plot(x, y)
		if not CyPlot.isImpassable():
			if CyPlot.isWater():
				waterPlots.append((x, y))
				iWater += 1
			else:
				landPlots.append((x, y))
				iLand += 1
		x += 1
		if x == dx:
			x = 0
			y += 1

	CyPlayer = GC.getPlayer(0)
	iWaterInc = 0
	iTotalWater = 0
	iLandInc = 0
	iTotalLand = 0
	iUnits = GC.getNumUnitInfos()
	iUnit = 0
	while iUnit < iUnits:
		bLand = False
		bWater = False
		DOMAIN = GC.getUnitInfo(iUnit).getDomainType()
		if DOMAIN == DomainTypes.DOMAIN_AIR:
			if iTotalWater + iLand < iTotalLand + iWater:
				if iWater:
					bWater = True
				elif iLand:
					bLand = True
			else:
				if iLand:
					bLand = True
				elif iWater:
					bWater = True

		elif DOMAIN == DomainTypes.DOMAIN_SEA:
			bWater = iWater != 0
		else:
			bLand = iLand != 0

		if bLand or bWater:
			if bLand:
				x, y = landPlots[iLandInc]
				iLandInc += 1
				if iLandInc == iLand:
					iLandInc = 0
				iTotalLand += 1
			else:
				x, y = waterPlots[iWaterInc]
				iWaterInc += 1
				if iWaterInc == iWater:
					iWaterInc = 0
				iTotalWater += 1
			CyPlayer.initUnit(iUnit, x, y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
		iUnit += 1
