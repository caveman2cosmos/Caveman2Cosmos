
from CvPythonExtensions import *
import Popup as PyPopup

GC = CyGlobalContext()
bDebugMode = False

def toggleDebugMode():
	global bDebugMode
	bDebugMode = not bDebugMode
	CyInterface().addImmediateMessage("Python Debug Mode: %s" % bDebugMode, "AS2D_ERROR")

class DebugUtils:
	def __init__(self):
		self.iLastUnitPicker = -1

	def resetUnitMovement(self):
		for iPlayer in xrange(GC.getMAX_PLAYERS()):
			CyPlayer = GC.getPlayer(iPlayer)
			CyUnit, i = CyPlayer.firstUnit(false)
			while CyUnit:
				CyUnit.setMoves(0)
				CyUnit, i = CyPlayer.nextUnit(i, false)

	def allBonuses(self):
		iNBonuses = GC.getNumBonusInfos()
		map = GC.getMap()
		if iNBonuses < map.getGridWidth() * map.getGridHeight():
			for x in xrange(map.getGridWidth()):
				for y in xrange((iNBonuses/map.getGridWidth())+1):
					map.plot(x,y).setBonusType((x + y * map.getGridWidth())%iNBonuses);

	def allImprovements(self):
		iNImprovements = GC.getNumImprovementInfos()
		map = GC.getMap()
		if (iNImprovements < map.getGridWidth() * map.getGridHeight()):
			for x in xrange(map.getGridWidth()):
				for y in xrange((iNImprovements/map.getGridWidth())+1):
					map.plot(x,y).setImprovementType((x + y * map.getGridWidth())%iNImprovements);


################ TRIGGERED EVENTS ################

	# Event 5
	def initEffectViewer(self, argsList):
		px, py = argsList
		pPlot = GC.getMap().plot(px,py)
		popup = PyPopup.PyPopup(5, EventContextTypes.EVENTCONTEXT_SELF)
		popup.setSize(550,300)
		popup.setUserData((px,py))
		popup.setHeaderString("Python Debug Tools: Object Placer")
		# Pulldown0 - Player Selection
		numEffects = GC.getNumEffectInfos()	# get total # of units from Game

		popup.createPythonPullDown("Choose an Effect")
		for i in xrange(GC.getNumEffectInfos()):
			popup.addPullDownString(GC.getEffectInfo(i).getType(), i)

		popup.createPythonEditBox("Default", "Modify the scale of the effect")
		popup.createPythonEditBox("Default", "Modify the update rate")

		# Launch Popup
		popup.launch()
		return 0

	def applyEffectViewer(self, iPlayer, userData, popupReturn):
		px, py = userData
		CyPlot = GC.getMap().plot(px, py)
		if not CyPlot.isNone():
			CyEngine().triggerEffect(popupReturn.getSelectedPullDownValue(0), CyPlot.getPoint())

	# Event 5002
	def initUnitPicker(self, argsList):
		px, py = argsList
		pPlot = GC.getMap().plot(px,py)
		popup = PyPopup.PyPopup(5002, EventContextTypes.EVENTCONTEXT_ALL)
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
		if CyPlayer.isNone():
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

	# Event 5003
	def initTechsCheat(self, argsList):
		popup = PyPopup.PyPopup(5003, EventContextTypes.EVENTCONTEXT_ALL)
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

	def applyTechCheat(self, iPlayer, userData, popupReturn):

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

	# Event 5012
	def initWonderMovie(self, argsList):
		popup = PyPopup.PyPopup(5012, EventContextTypes.EVENTCONTEXT_ALL)
		popup.setHeaderString("Wonder Movie")
		popup.createPullDown()
		for i in xrange(GC.getNumBuildingInfos()):
			szMovieFile = GC.getBuildingInfo(i).getMovie()
			if (szMovieFile != None and len(szMovieFile) > 0):
				popup.addPullDownString(GC.getBuildingInfo(i).getDescription(), i)

		for i in xrange(GC.getNumProjectInfos()):
			szMovieFile = None
			szArtDef = GC.getProjectInfo(i).getMovieArtDef()
			if (len(szArtDef) > 0):
				szMovieFile = CyArtFileMgr().getMovieArtInfo(szArtDef).getPath()
			if (szMovieFile != None and len(szMovieFile) > 0):
				popup.addPullDownString(GC.getProjectInfo(i).getDescription(), GC.getNumBuildingInfos() + i)

		popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)

	def applyWonderMovie(self, iPlayer, userData, popupReturn):

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

debugUtils = DebugUtils()


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
