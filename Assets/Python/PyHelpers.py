## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil

gc = CyGlobalContext()

class PyPlayer:
	' CyPlayer Helper Functions - Requires Player ID to initialize instance '

	def __init__(self, iPlayer):
		' Called whenever a new PyPlayer instance is created '
		if iPlayer:
			self.player = gc.getPlayer(iPlayer)
		else:
			self.player = gc.getPlayer(0)

	def CyGet(self):
		' used to get the CyUnit instance for quick calls '
		return self.player

	def getPlayer(self):
		return self.player

	def isNone(self):
		' bool - Is the CyPlayer Instance Valid? '
		return self.player.isNone()

	def isAlive(self):
		return self.player.isAlive()

	def getID(self):
		' int - ID # '
		return self.player.getID()

	def getName(self):
		return self.player.getName()

# Players Team
	def getTeamID(self):
		' int - gets the players teamID '
		return self.player.getTeam()

	def getTeam(self):
		' obj - returns Team Instance '
		return gc.getTeam( self.getTeamID() )

	def canTradeNetworkWith(self, iPlayer):
		return self.player.canTradeNetworkWith(iPlayer)

# AI
	def AI_getAttitude(self, iTeam):
		return self.player.AI_getAttitude(iTeam)

	def AI_changeAttitude(self, iPlayer, amount):
		return self.player.AI_changeAttitudeExtra(iPlayer, amount)

# Players Gold
	def getGold(self):
		return self.player.getGold()

	def changeGold(self, iGold):
		self.player.changeGold(iGold)

	def setGold(self, iGold):
		' none - Sets Gold to iGold '
		self.player.setGold( iGold )

	def getTotalMaintenance(self):
		return self.player.getTotalMaintenance()

	def calculateUnitSupply(self):
		return self.player.calculateUnitSupply()

# Players Yields / Commerce

	def calculateResearchRate(self):
		' int - Total Research Rate per Turn '
		return self.player.calculateResearchRate( TechTypes.NO_TECH )

	def getGoldPerTurn(self):
		return self.player.getGoldPerTurn()

# Players Research
	def getResearchedTechList(self):
		' intlist - list of researched techs '
		lTechs = []
		for i in range(gc.getNumTechInfos()):
			if self.hasResearchedTech(i):
				lTechs.append(i)
		return lTechs

	def hasResearchedTech(self, iTech):
		' bool - Has researched iTech '
		if self.getTeam().isHasTech( iTech ):
			return True

	def setHasTech(self, iTech):
		' gives the player iTech '
		#int /*TechTypes*/ eIndex, bool bNewValue, int /*PlayerTypes*/ ePlayer, bool bFirst, bool bAnnounce
		self.getTeam().setHasTech( iTech, True, self.getID(), False, False )

	def getResearchTurnsLeft(self, iTech, bOverflow = 1):
		' int '
		return self.player.getResearchTurnsLeft( iTech, bOverflow )

	def getCurrentTechName(self):
		' str - Current Research Tech Name '
		iTech = self.player.getCurrentResearch()
		if ( iTech > 0 and iTech < gc.getNumTechInfos() ):
			return str( gc.getTechInfo( iTech ).getDescription() )
		# "No Research"
		return "No Research"

	def isBarbarian(self):
		return self.player.isBarbarian()

	def isNPC(self):
		return self.player.isNPC()

	def isHominid(self):
		return self.player.isHominid()

	def isMinorCiv(self):
		return self.player.isMinorCiv()

	def getNumTradeableBonuses(self, iBonus):
		return self.player.getNumTradeableBonuses(iBonus)

	def calculateInflatedCosts(self):
		return self.player.calculateInflatedCosts()

	def calculateGoldRate(self):
		return self.player.calculateGoldRate()

# Players Civ
	def getCivilizationInfo(self):
		' CivilizationInfo - Civ Info instance '
		return gc.getCivilizationInfo( self.player.getCivilizationType() )

	def getCivDescription(self):
		' str - Civ Description '
		return self.player.getCivilizationDescription(0)

	def getCivilizationName(self):
		' str - Civ Description '
		return self.player.getCivilizationDescription(0)

	def getCivilizationShortDescription(self):
		' str - Civ Description '
		return self.player.getCivilizationShortDescription(0)

	def getCivilizationAdjective(self):
		' str - Civ Ajective '
		return self.player.getCivilizationAdjective(0)

# Players LeaderHead
	def getLeaderName(self):
		return self.getLeaderHeadInfo().getDescription()

	def getLeaderType(self):
		return self.player.getLeaderType()

	def getLeaderHeadInfo(self):
		' LeaderHeadInfo - Leaders info instance '
		return gc.getLeaderHeadInfo( self.player.getLeaderType() )

	def getLeaderHeadDescription(self):
		' str - Leader Name '
		return self.getLeaderHeadInfo().getDescription()

	def getLeaderHeadImage(self):
		"str - location of the regular size leaderhead image"
		return self.getLeaderHeadInfo().getLeaderHead()

# Players Traits
	def getTraitList(self):
		' intList - Trait List '
		lTrait = []
		for i in range( gc.getNumTraitInfos() ):
			if ( self.getLeaderHeadInfo().hasTrait(i) ):
				lTrait.append(i)
		return lTrait

	def getTraitInfos(self):
		' TraitInfoList '
		lTrait = []
		for iTrait in self.getTraitList():
			lTrait.append( gc.getTraitInfo(iTrait) )
		return lTrait

# Players Civics
	def getCurrentCivicByOption(self, iCivicOption):
		' int - current civic for iCivicOption '
		return self.player.getCivics( iCivicOption )

	def getCurrentCivicList(self):
		' intList - list of current Civic IDs '
		lCivics = []
		for i in range( CyGlobalContext().getNumCivicOptionInfos() ):
			lCivics.append( self.player.getCivics( i ) )
		return lCivics

	def getCurrentCivicDescriptions(self):
		' strList - description list of current civics '
		lCivics = self.getCurrentCivicList()
		for i in range( len(lCivics) ):
			lCivics[i] = CyGlobalContext().getCivicInfo( lCivics[i] ).getDescription()
		return lCivics

	def getCivicUpkeep(self):
		' int - total Civic Upkeep '
		return self.player.getCivicUpkeep([], False)	# pass in an empty list

# Players Religion
	def getStateReligion(self):
		return self.player.getStateReligion()

# Players Units
	def getUnitList(self):
		' UnitList - All of the players alive units '
		lUnit = []
		(loopUnit, iter) = self.player.firstUnit(False)
		while( loopUnit ):
			if ( not loopUnit.isDead() ): #is the unit alive and valid?
				lUnit.append(loopUnit) #add unit instance to list
			(loopUnit, iter) = self.player.nextUnit(iter, False)
		return lUnit

	def getNumUnits(self):
		return self.player.getNumUnits()

	def getUnitByScriptData(self, scriptData):
		for unit in self.getUnitList():
			if unit.getScriptData() == scriptData:
				return unit

		return 0

	def initUnit(self, unitID, X, Y, iNum = 1):
		"none - spawns unitIdx at X, Y - ALWAYS use default UnitAIType"
		if (iNum > 1): #multiple units
			for i in range(iNum):
				self.player.initUnit(unitID, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
		else:
			self.player.initUnit(unitID, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)

	def hasUnitType(self, iUnit):
		' bool - Has iUnit? '
		for unit in self.getUnitList():
			if ( unit.getUnitType() == iUnit ):
				return True

	def getUnitsOfType(self, iUnit):
		' UnitList - of iUnit '
		lUnits = []
		for unit in self.getUnitList():
			if ( unit.getUnitType() == iUnit ):
				lUnits.append(unit)
		return lUnits

	def getNumUnitsLevel(self, iLevel):
		' int - num units at iLevel '
		iCounter = 0
		for unit in self.getUnitList():
			if ( unit.getLevel() >= iLevel ):
				iCounter+=1
		return iCounter

# Players Cities
	def getTotalPopulation(self):
		return self.player.getTotalPopulation()

	def getCityList(self):
		' PyCitylist - list of PyCity player owns '
		lCity = []
		(loopCity, iter) = self.player.firstCity(False)
		while(loopCity):
			cityOwner = loopCity.getOwner()
			if loopCity.getOwner() == self.getID(): #only valid cities
				city = PyCity( self.getID(), loopCity.getID() )
				lCity.append(city)
			(loopCity, iter) = self.player.nextCity(iter, False)
		return lCity

	def getNumCities(self):
		return self.player.getNumCities()

	def isFoundedFirstCity(self):
		return self.player.isFoundedFirstCity()

	def initCity(self, iX, iY):
		' adds city to iX, iY '
		self.player.initCity(iX, iY)

	def getCapitalCity(self):
		' PyCity - capital '
		for city in self.getCityList():
			if ( city.GetCy().isCapital() ):
				return city

	def getCity(self, iCity):
		' PyCity - returns city iCity '
		return self.player.getCityList()[iCity]

# Players Plots
	def getPlotList(self):
		' plotList - player plots '
		lPlots = []
		for iIndex in self.getPlotIDList():
			lPlots.append( CyMap().plotByIndex(i) )
		return lPlots

	def getTotalLand(self):
		return self.player.getTotalLand()

	def getPlotIDList(self):
		' intList - player plots indexes '
		lPlotIDs = []
		for i in xrange( CyMap().numPlots() ):
			if ( CyMap().sPlotByIndex( i ).getOwner() == self.getID() ):
				lPlotIDs.append( i )
		return lPlotIDs

	def getNumPlots(self):
		' int - numOwned plots '
		return len( self.getPlotIDList() )

	def getPlotsWithBonus(self, iBonus):
		' plotList with iBonus '
		lPlots = []
		for plot in self.getPlotList():
			if ( not plot.getBonusType() == BonusTypes.NO_BONUS and plot.getBonusType() == iBonus ):
				lPlots.append(plot)
		return lPlots

	def getNumImprovementPlots(self):
		' int numPlots with iImprovement '
		iCounter = 0
		for plot in self.getPlotList():
			if ( not plot.getImprovementType() == ImprovementTypes.NO_IMPROVEMENT ):
				iCounter += 1
		return iCounter


class PyCity:
	"requires player instance & cityID"
	def __init__(self, iPlayerID, iCityID):
		self.player = gc.getPlayer(iPlayerID)
		self.city = self.player.getCity(iCityID)

	def isNone(self):
		"bool - Is the city instance valid?"
		return self.city is None

	def GetCy(self):
		' Cy instance of city '
		return self.city

#################### G E N E R A L	 C I T Y	 F U N C T I O N S ####################

	def setName(self, newName):
		"none - Set Cities Name"
		return self.city.setName(newName, False)

	def getAngryPopulation(self):
		"int - Angry Population"
		return self.city.angryPopulation(0)

	def getHappyPopulation(self):
		"int - Happy Population"
		return self.city.happyLevel()

	def getUnhappyPopulation(self):
		"int - Unhappy Population"
		return self.city.unhappyLevel(0)

	def canTrain (self, iUnit):
		return self.city.canTrain(iUnit, False, False, False, False)

	def canConstruct (self, iBuilding):
		return self.city.canConstruct(iBuilding, False, False, False)

	def canCreate (self, iProject):
		return self.city.canCreate(iProject, False, False)

	def canMaintain (self, iItem):
		return self.city.canMaintain(iItem, False)

	def getProductionTurnsLeft(self):
		"int - Turns Remaining"
		if self.isProductionProcess():
			return "-"
		return self.city.getProductionTurnsLeft()

	def getCultureCommerce(self):
		return self.city.getCommerceRate(CommerceTypes.COMMERCE_CULTURE)

	def getCulture(self):
		"int - Culture points"
		return self.city.getCulture(self.player.getID())

	def changeCulture(self, iChange, bPlots=True):
		return self.city.changeCulture(self.player.getID(), iChange, bPlots)

	def setCulture(self, iChange, bPlots=True):
		self.city.setCulture(self.player.getID(), iChange, bPlots)

	def initUnit(self, iUnitID):
		plot = self.plot()
		gc.getActivePlayer().initUnit( iUnitID, plot.getX(), plot.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION )

	def getNumBuildingIdx(self, buildingIdx):
		"int - How many buildingIdx city has?"
		if (self.city.getNumBuilding(buildingIdx)):
			return True
		return False

	def getHolyCity(self):
		lHolyCity = []
		for i in range(gc.getNumReligionInfos()):
			if self.city.isHolyCityByType(i):
				lHolyCity.append(i)
		return lHolyCity

	def getReligions(self):
		lReligions = []
		for i in range(gc.getNumReligionInfos()):
			if self.hasReligion(i):
				lReligions.append(i)
		return lReligions

	def hasCorporation(self, corporationIdx):
		"bool - City has corporationIdx?"
		if (self.city.isHasCorporation(corporationIdx)):
			return True
		return False

	def getHeadquarters(self):
		lHeadquarters = []
		for i in range(gc.getNumCorporationInfos()):
			if self.city.isHeadquartersByType(i):
				lHeadquarters.append(i)
		return lHeadquarters

	def getCorporations(self):
		lCorporations = []
		for i in range(gc.getNumCorporationInfos()):
			if self.hasCorporation(i):
				lCorporations.append(i)
		return lCorporations

	def getBuildingList(self):
		"intList - List of all buildingIdx in the city"
		numBuildingInfos = gc.getNumBuildingInfos()
		buildingList=[]
		for buildingIdx in range(numBuildingInfos):
			if (self.getNumBuildingIdx( buildingIdx )):
				buildingList.append( buildingIdx )
		return buildingList

	def getBonusList(self):
		"intList - all bonuses connected to city"
		lBonus = []
		for i in range(gc.getNumBonusInfos()):
			if self.hasBonusIdx( i ):
				lBonus.append(i)
		return lBonus

class PyInfo:

	class UnitInfo:
		"Unit Info helper class"
		def __init__(self, unitTypeID):
			self.unitTypeID = unitTypeID
			self.info = gc.getUnitInfo(self.unitTypeID)

		def isValid(self):
			if self.info:
				return True
			return False

		def getID(self):
			return self.unitTypeID

		def getDescription(self):
			"str - Units Name"
			return self.info.getDescription()

		def getButton(self):
			"str - Units KFM Button - DDS Format"
			return CyArtFileMgr().getUnitArtInfo(self.info.getArtDefineTag(0)).getButton()

		def isAnimal(self):
			"bool - an animal?"
			if self.info.isAnimal():
				return True
			return False

		def isFoodProduction(self):
			"bool - requires food production?"
			if self.info.isFoodProduction():
				return True
			return False

		def getCombatStrength(self):
			"int - base combat strength"
			return self.info.getCombat()

		def getMoves(self):
			"int - base unit movements"
			return self.info.getMoves()

		def getProductionCost(self):
			"int - production cost"
			return self.info.getProductionCost()

		def getDomainType(self):
			"int - domain type"
			return self.info.getDomainType()

		def getUnitCombatType(self):
			"int - combat type"
			return self.info.getUnitCombatType()

		def getUnitAITypes(self, AIType):
			"objlist - list of units of AIType"
			UnitAIList=[]
			for i in range(UnitAITypes.NUM_UNITAI_TYPES):
				if self.info.getUnitAITypes(i) == AIType:
					UnitAIList.append(i)
			return UnitAIList

		def isTechPrereq(self,techID):
			"bool - is techID a prereq of this tech?"
			if techID == self.getTechPrereqID():
				return True
			elif techID in self.getPrereqOrTechIDList():
				return True
			return False

		def isLatestTechPrereq(self,techID):
			pTechOr = self.getPrereqOrTechIDList()
			if pTechOr:
				pTechOr = pTechOr.sort()
				for i in range(len(pTechOr)):
					if techID < pTechOr[i]:
						return True
			return False

		def getTechPrereq(self):
			"str - prerequisite technology"
			pTech = self.info.getPrereqAndTech()
			if pTech >= 0 and pTech < gc.getNumTechInfos():
				return TechnologyInfo(pTech).getDescription()

		def getTechPrereqID(self):
			"int - prerequisite techs xml id"
			pTech = self.info.getPrereqAndTech()
			if pTech >= 0 and pTech < gc.getNumTechInfos():
				return pTech
			return -1

		def getTechPrereqButton(self):
			"str - location of techs button art"
			pTech = self.getTechPrereqID()
			if pTech and not pTech == 0:
				return TechnologyInfo(pTech).getButton()

		def getPrereqOrTechIDList(self):
			"intList - IDList of multiple tech requirements"
			pTechIDList = []
			for i in range(gc.getDefineINT("NUM_UNIT_OR_TECH_PREREQS")):
				iResult = self.info.getPrereqOrTechs(i)
				if iResult >= 0:
					pTechIDList.append(iResult)
			return pTechIDList

		def getPrereqOrTechInfoList(self):
			"objList - Info list of IDList items"
			pTechIDList = self.getPrereqOrTechIDList()
			pTechInfos = []
			for i in range(len(pTechIDList)):
				pTechInfos.append(UnitInfo(pTechIDList[i]))
			return pTechInfos

		def getPrereqBonusID(self):
			"int - required bonus ID"
			if self.info.getPrereqAndBonus():
				return self.info.getPrereqAndBonus()
			return -1

		def getPrereqBonusInfo(self):
			"obj - info for required bonus ID"
			pBonus = self.info.getPrereqAndBonus()
			if pBonus >= 0 and pBonus <= gc.getNumBonusInfos():
				return pBonus

		def getPrereqBonusIcon(self):
			"str - prerequisite bonus"
			pBonus = self.info.getPrereqAndBonus()
			if pBonus >= 0 and pBonus < gc.getNumBonusInfos():
				return "%c" %(BonusInfo(pBonus).getSymbol())
			return ""

		def getPrereqBonusIDList(self):
			"intList - ID list of multiple bonus requirements"
			preqBonusIDList = []
			for i in range(gc.getNUM_UNIT_PREREQ_OR_BONUSES()):
				iResult = self.info.getPrereqOrBonuses(i)
				if iResult >=0:
					preqBonusIDList.append(iResult)
			return preqBonusIDList

		def getPrereqBonusInfoList(self):
			"objList - Info list of multiple bonus requirements"
			bInfos = []
			pBonuses = self.getPrereqBonusIDList()
			for item in pBonuses:
				bInfos.append(UnitInfo(item))
			return bInfos

		def getPrereqBonusIcons(self):
			"str - string with all required bonuses"
			bonusIcons = ""
			pBonuses = self.getPrereqBonusIDList()
			for i in range(len(pBonuses)):
				loopID = pBonuses[i]
				loopIcon = BonusInfo(loopID).getSymbol()
				bonusIcons+= loopIcon
			return bonusIcons


	class TechnologyInfo:
		"Technology Info class for easier manipulation of this object"
		def __init__(self, techID):
			self.techID = techID
			self.info = gc.getTechInfo(techID)

		def isValid(self):
			if self.info:
				return True
			return False

		def getID(self):
			"int - tech instances id"
			return self.techID

		def getDescription(self):
			"str - techs name"
			return self.info.getDescription()

		def getButton(self):
			"str - location of kfm button"
			if self.isValid():
				return self.info.getButton()
			return 0

		def createPrereqDictionary(self):
			pDict = {}
			for i in range(gc.getNumTechInfos()):
				TechInfo = TechnologyInfo(i)
				pDict[i]=(i,"%s" %(TechInfo.getDescription()), TechInfo.getTechPreqIdxList())
			return pDict

		def createNamePrereqDictionary(self):
			pDict = {}
			f = file('TechPrereq.txt', "w")
			f.write("### CIV Tech Prereq List ###\n\n")
			for i in range(gc.getNumTechInfos()):
				lInfo = TechnologyInfo(i)
				f.write("\n   (%d)%s - %s" %(i, lInfo.getDescription(), lInfo.getTechPreqDescList()))
			f.close()

		def getTechPreqIdxList(self):
			"intList - list of prerequisite techs in nested forum ((mining,agriculture),(agriculture,thewheel))"
			techInfo = self.info
			preqList = []
			info = ()
			for i in range(4):
				for j in range(4):
					result = techInfo.getTechPrereqs(i,j)
					if not result == -1:
						info = info + (result,)
					if j == 3 and not len(info) < 1:
						if len(info) == 0:
							pass
						preqList.append(info)
						info = ()
			return preqList

		def getTechPreqDescList(self):
			lPrereqID=self.getTechPreqIdxList()
			lPrereqDesc = []
			lInfo = []
			for i in range(len(lPrereqID)):
				lGroup = lPrereqID[i]
				if len(lGroup) > 0:
					for j in range(len(lPrereqID[i])):
						lGroup2 = lPrereqID[i][j]
						lInfo.append(TechnologyInfo(lGroup2).getDescription())
				lPrereqDesc.append(lInfo)
				lInfo=[]
			return lPrereqDesc

		def getTechPreqInfoList(self):
			"objList - takes the ID functions and returns them in info form"
			preqList = self.getTechPreqIdxList()
			infoList = []
			infoGroup = ()
			if preqList:
				for i in range(len(preqList)):
					loopGroup = preqList[i]
					for j in range(len(loopGroup)):
						loopID = loopGroup[j]
						loopInfo = TechnologyInfo(loopID)
						if loopInfo:
							infoGroup = infoGroup + (loopInfo,)
					if infoGroup:
						infoList.append(infoGroup)
						infoGroup = ()
			return infoList

		def getTechPreqButtonList(self):
			"objList - list of preqtechs buttons locations"
			pTechButtons = []
			for info in self.getTechPreqInfoList():
				button = info.getButton()
				pTechButtons.append(button)
			return pTechButtons

		def getResearchCost(self):
			"int - research cost"
			return self.info.getResearchCost()

		def getiEra(self):
			"int - the techs era"
			return self.info.getEra()

		def isWorkerSpeedModifier(self):
			"bool - tech modify worker speed?"
			return self.info.iWorkerSpeedModifier

		def getTradeRouteModifer(self):
			"int - trade route modifier"
			if self.info.iTradeRoutes:
				return self.info.iTradeRoutes
			return 0

		def getWorkerSpeedModifier(self):
			"int - how much are they modified?"
			if self.info.iWorkerSpeedModifier > 0:
				return self.info.iWorkerSpeedModifier
			return 0

		def isGoodyTech(self):
			"bool - can this tech be researched from popping a goodie hut?"
			if self.info.bGoodyTech:
				return True
			return False

		def getHealthBonus(self):
			if self.info.getHealth():
				return self.info.getHealth()
			return 0

		def getUnlockedBuildingIdxList(self):
			"intList - IDList of unlocked Buildings"
			buildingList=[]
			numBuilding = gc.getNumBuildingInfos()
			for i in range(numBuilding):
				loopBuilding = gc.getBuildingInfo(i)
				if loopBuilding.getPrereqAndTech() == self.techID:
					buildingList.append(i)
			return buildingList

		def getListUnlockedBuildingInfos(self):
			"objList - info list of unlocked buildings"
			idList = self.getUnlockedBuildingIdxList()
			buildingList = []
			for i in range(len(idList)):
				buildingList.append(BuildingInfo(idList[i]))
			return buildingList

		def getUnlockedUnitIdxList(self):
			"intList - IDList of unlocked Units"
			unitList=[]
			for i in range(gc.getNumUnitInfos()):
				loopUnit = UnitInfo(i)
				if loopUnit.isTechPrereq(self.techID):
					unitList.append(i)
			return unitList

		def getListUnlockedUnitInfos(self):
			"objList - info list of unlocked units"
			unitList = []
			for i in range(len(self.getUnlockedUnitIdxList())):
				unitList.append(UnitInfo(idList[i]))
			return unitList

		def getUnlockedReligionIdxList(self):
			"intList - IDList of unlocked Religions"
			religionlist = []
			for i in range(gc.getNumReligionInfos()):
				if ( gc.getReligionInfo(i).getTechPrereq() == self.techID ):
					religionList.append(i)
			return religionList

		def getReligionButton(self):
			"str - religion icon"
			for i in range(gc.getNumReligionInfos()):
				if ( gc.getReligionInfo(i).getTechPrereq() == self.techID ):
					return "%c" %(gc.getReligionInfo(i).getChar(),)
			return 0

		def isBuildTech(self):
			"bool - unlocks build"
			for i in range(gc.getNumBuildInfos()):
				if ( BuildInfo(i).getTechPrereq() == self.techID ):
					return True
			return False

		def getUnlockedBuildInfos(self):
			"objList - list of build infos the tech unlocks"
			lBuilds = []
			for i in range(gc.getNumBuildInfos()):
				if ( BuildInfo(i).getTechPrereq() == self.techID ):
					lBuilds.append(BuildInfo(i))
			return lBuilds

		def isCivicTech(self):
			"bool - tech unlocks Civic?"
			for i in range(gc.getNumCivicInfos()):
				if ( gc.getCivicInfo(i).getTechPrereq() == self.techID ):
					return True
			return False

		def getUnlockedCivicInfos(self):
			"objList - list of civic infos the tech unlocks"
			lCivics=[]
			for i in range(gc.getNumCivicInfos()):
				if ( gc.getCivicInfo(i).getTechPrereq() == self.techID ):
					lCivics.append(i)
			return lCivics


	class BuildingInfo:
		"Building Info helper class"
		def __init__(self, buildingID):
			self.buildingID = buildingID
			self.info = gc.getBuildingInfo(self.buildingID)

		def isValid(self):
			if self.info:
				return True
			return False

		def getID(self):
			return self.buildingID

		def getDescription(self):
			return self.info.getDescription()

		def getButton(self):
			return CyArtFileMgr().getBuildingArtInfo(self.info.getArtDefineTag()).getButton()

		def getTechPrereq(self):
			return self.info.getPrereqAndTech()
