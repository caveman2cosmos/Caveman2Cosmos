## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
#
# CIV Python Tools Code
#
from CvPythonExtensions import *
import CvUtil
import PyHelpers
import Popup as PyPopup

PyPlayer = PyHelpers.PyPlayer
PyGame = PyHelpers.PyGame

# globals
GC = CyGlobalContext()
iLastSelectedObject = -1
gSetUnit = 0

def toggleDebugMode():
	return

def adjustUnitMovement(iPlayer, iUnitType, iNumMoves):
	player = PyPlayer(iPlayer)
	for unit in player.getUnitList():
		if ( int(unit.getUnitType()) == iUnitType ):
			unit.setMoves(iNumMoves)

def giveUnitsLotsOfMoves():
	playerTeam = GC.getActivePlayer().getTeam(0)
	playerTeam.changeExtraMoves(DomainTypes.DOMAIN_LAND, 1000)
	playerTeam.changeExtraMoves(DomainTypes.DOMAIN_SEA, 1000)
	playerTeam.changeExtraMoves(DomainTypes.DOMAIN_AIR, 1000)

############################### DEBUG TOOLS ####################################
class CvDebugUtils:
	def __init__(self):
		self._bDebugMode = False
		self.iActiveEffect = -1
		self.pEffectPlot = None

	def getDebugMode( self ):
		return self._bDebugMode

	def setDebugMode( self, bVal ):
		self._bDebugMode = bVal
		CyInterface().addImmediateMessage( "CvDebugUtils.setDebugMode set to %s" % self.bDebugMode, "" )

	def notifyInput( self, argsList ):
		#print "Python Debug Mode Notify"
		return 0

	def initEffectViewer( self, argsList ):
		px,py = argsList
		pPlot = CyMap().plot(px,py)
		popup = PyPopup.PyPopup(5, EventContextTypes.EVENTCONTEXT_SELF )
		popup.setSize(550,300)
		popup.setUserData( (px,py) )
		popup.setHeaderString( "Python Debug Tools: Object Placer" )
		# Pulldown0 - Player Selection
		numEffects = GC.getNumEffectInfos()	# get total # of units from Game

		popup.createPythonPullDown("Choose an Effect")
		for i in range(GC.getNumEffectInfos()):
			popup.addPullDownString(GC.getEffectInfo(i).getType(), i)

		popup.createPythonEditBox( "Default", "Modify the scale of the effect" )
		popup.createPythonEditBox( "Default", "Modify the update rate" )

		# Launch Popup
		popup.launch()
		return 0

	def applyEffectViewer(self, playerID, userData, popupReturn):
		px,py = userData
		self.pEffectPlot = CyMap().plot(px,py)

		if self.pEffectPlot.isNone():
			return 0

		self.iActiveEffect = popupReturn.getSelectedPullDownValue( 0 )

		CyEngine().triggerEffect(self.iActiveEffect, self.pEffectPlot.getPoint())
		#scale = popupReturn.getSelectedListBoxValue( 0 )
		#updateRate = int( popupReturn.getEditBoxString( 0 ) )

	############################
	## UNIT / CITY PLAYER
	############################
	def initUnitPicker( self, argsList ):
		'initUnitPicker - for placing units & cities'
		px,py = argsList
		pPlot = CyMap().plot(px,py)
		popup = PyPopup.PyPopup(5002, EventContextTypes.EVENTCONTEXT_ALL )
		popup.setSize(400,600)
		popup.setPosition(600,25)
		popup.setUserData( (px,py) )
		popup.setHeaderString( "Python Debug Tools: Object Placer" )
		#popup.setBodyString( "Choose Player:" )

		# Pulldown0 - Player Selection
		iNumUnits = GC.getNumUnitInfos()	# get total # of units from Game
		iOwner = pPlot.getOwner()
		if ( iOwner == PlayerTypes.NO_PLAYER ):
			iOwner = GC.getGame().getActivePlayer()
		popup.createPythonPullDown("Choose a Player")
		popup.addPullDownString(GC.getPlayer(iOwner).getName(), iOwner)
		for i in range(GC.getMAX_PLAYERS()):
			if (GC.getPlayer(i).isEverAlive()):
				if (i != iOwner):
					popup.addPullDownString(GC.getPlayer(i).getName(), i)

		popup.addSeparator()

		# ListBox0 - Unit List w/ City also selectable
		#popup.setBodyString( "Select Game Object to Add:" )

		popup.createPythonListBox( "" )
		popup.addListBoxString( 'Nothing', iNumUnits + 1 )   # for clean exit
		popup.addSeparator()
		lastSelection = u""
		if ( iLastSelectedObject == -1 ):
			pass
		elif ( iLastSelectedObject == iNumUnits+1 ):
			lastSelection = u'Nothing'
		elif ( iLastSelectedObject == iNumUnits ):
			lastSelection = u'City'
		else:
			lastSelection = GC.getUnitInfo(iLastSelectedObject).getDescription()
		if ( not iLastSelectedObject == -1 ):
			popup.addListBoxString( lastSelection, iLastSelectedObject )
		popup.addListBoxString( u'City', iNumUnits )    	# list City first

		# sort units alphabetically
		unitsList=[(0,0)]*iNumUnits
		for j in range( iNumUnits ):
			unitsList[j] = (GC.getUnitInfo(j).getDescription(), j)
		unitsList.sort()

		for j in range( iNumUnits ):
			popup.addListBoxString( unitsList[j][0], unitsList[j][1])

		# EditBox0 - Customize how many units to build
		#popup.setBodyString( "How many objects?" )
		popup.createPythonEditBox( "1", "This allows you to create multiple units." )

		# Launch Popup
		#popup.setSize( 400, 600 )
		popup.launch()
		return 0

	def applyUnitPicker( self, argsList ):
		'Apply Unit Picker'
		popupReturn, userData = argsList
		px,py = userData
		pPlot = CyMap().plot(px,py)

		if pPlot.isNone():
			return 0

		# UNIT DEBUG MENU
		playerID = popupReturn.getSelectedPullDownValue( 0 )
		selectedObject = popupReturn.getSelectedListBoxValue( 0 )
		iSpawnNum = int( popupReturn.getEditBoxString( 0 ) )

		player = PyPlayer( playerID )
		if ( player.isNone() ):
			return -1   # Error

		iNumUnits = GC.getNumUnitInfos()
		global iLastSelectedObject
		iLastSelectedObject = selectedObject
		if ( selectedObject != iNumUnits + 1 ):# Nothing

			if ( selectedObject == iNumUnits ):# City"
				player.initCity( px,py )

			else:
				player.initUnit( selectedObject, px, py, iSpawnNum )
		else:
			iLastSelectedObject = -1

		return 0

	############################
	## TECH / GOLD CHEAT POPUP
	############################
	def cheatTechs( self ):
		'Cheat techs and gold to the players'
		popup = PyPopup.PyPopup(5003, EventContextTypes.EVENTCONTEXT_ALL )
		popup.setHeaderString( "Tech & Gold Cheat!" )
		popup.createPullDown()
		popup.addPullDownString( "All", GC.getMAX_CIV_PLAYERS() )
		for i in range( GC.getMAX_CIV_PLAYERS() ):
			if ( GC.getPlayer(i).isAlive() ):
				popup.addPullDownString( GC.getPlayer(i).getName(), i )
		popup.setBodyString( "Modify Player %s:" %( CvUtil.getIcon('gold'),) )
		popup.createPythonEditBox( "0", "Integer value (positive or negative)" )

		# Loop through Era Infos and add names
		for i in range(GC.getNumEraInfos()):
			popup.addButton(GC.getEraInfo(i).getDescription())

		popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)

	def applyTechCheat( self, argsList ):
		'Apply Tech Cheat'
		popupReturn = argsList
		playerID = popupReturn.getSelectedPullDownValue( 0 )
		bAll = 0
		if playerID == GC.getMAX_CIV_PLAYERS():
			bAll = 1
			player = PyGame().getCivPlayerList()
		else:
			player = PyPlayer( playerID )
		era = popupReturn.getButtonClicked()

		try:
			goldChange = int( popupReturn.getEditBoxString( 0 ) )
		except:
			return 0

		if goldChange:
			if not bAll:
				player.changeGold(goldChange)
			else:
				for i in range(len(player)):
					player[i].changeGold(goldChange)

		for tech in PyGame().getEraTechList(era):
			id = tech.getID()
			if not bAll:
				player.setHasTech( id )
			else:
				for j in range(len(player)):
					player[j].setHasTech( id )
	def RotateUnit(self, Direction, px, py ):
		if ( px != -1 and py != -1 ):
			unit = CyMap().plot(px, py).getUnit(0)
			if ( not unit.isNone() ):
				unitEntity = CyUnitEntity(unit)
				dir = unitEntity.GetUnitFacingDirection( )
				dir += Direction * 0.05;
				unitEntity.SetUnitFacingDirection( dir )

	def resetUnitMovement( self ):
		global g_bDebugMode
		if g_bDebugMode == 0:
			return
		for i in range(GC.getMAX_PLAYERS()):
			(unit, iter) = GC.getPlayer(i).firstUnit(false)
			while (unit):
				unit.setMoves(0)
				(unit, iter) = GC.getPlayer(i).nextUnit(iter, false)

	def allUnits( self ):
		self.putOneOfEveryUnit();

	def allBonuses( self ):
		iNBonuses = GC.getNumBonusInfos()
		map = CyMap()
		if ( iNBonuses < map.getGridWidth() * map.getGridHeight() ):
			for x in range(map.getGridWidth()):
				for y in range((iNBonuses/map.getGridWidth())+1):
					map.plot(x,y).setBonusType( (x + y * map.getGridWidth())%iNBonuses );

	def allImprovements( self ):
		iNImprovements = GC.getNumImprovementInfos()
		map = CyMap()
		if ( iNImprovements < map.getGridWidth() * map.getGridHeight() ):
			for x in range(map.getGridWidth()):
				for y in range((iNImprovements/map.getGridWidth())+1):
					map.plot(x,y).setImprovementType( (x + y * map.getGridWidth())%iNImprovements );

	def putOneOfEveryUnit( self ):
		pass
		iNUnits = GC.getNumUnitInfos()
		map = CyMap()
		player = GC.getPlayer(0)
		if ( iNUnits < map.getGridWidth() * map.getGridHeight() ):
			for x in range(map.getGridWidth()):
				for y in range((iNUnits/map.getGridWidth())+1):
					player.initUnit( (x + y * map.getGridWidth())%iNUnits, x, y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION )

	def wonderMovie(self):
		popup = PyPopup.PyPopup(5012, EventContextTypes.EVENTCONTEXT_ALL)
		popup.setHeaderString( "Wonder Movie" )
		popup.createPullDown()
		for i in range(GC.getNumBuildingInfos()):
			szMovieFile = GC.getBuildingInfo(i).getMovie()
			if (szMovieFile != None and len(szMovieFile) > 0):
				popup.addPullDownString( GC.getBuildingInfo(i).getDescription(), i )

		for i in range(GC.getNumProjectInfos()):
			szMovieFile = None
			szArtDef = GC.getProjectInfo(i).getMovieArtDef()
			if (len(szArtDef) > 0):
				szMovieFile = CyArtFileMgr().getMovieArtInfo(szArtDef).getPath()
			if (szMovieFile != None and len(szMovieFile) > 0):
				popup.addPullDownString( GC.getProjectInfo(i).getDescription(), GC.getNumBuildingInfos() + i )

		popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)

	def applyWonderMovie( self, argsList ):
		'Apply Wonder Movie'
		popupReturn = argsList
		wonderID = popupReturn.getSelectedPullDownValue( 0 )

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

g_CvDebugUtils = CvDebugUtils()
