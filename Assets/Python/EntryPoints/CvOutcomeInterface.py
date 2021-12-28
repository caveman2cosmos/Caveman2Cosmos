from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()


################ CAPTIVES AND SLAVERY ###################

def doRemoveWVSlavery(argsList):
	unit = argsList[0]

	if not unit: return # False call

	iPlayer = unit.getOwner()
	player = GC.getPlayer(iPlayer)

	if not player.isAlive():
		return

	iWVSlavery = GC.getInfoTypeForString("BUILDING_WV_SLAVERY")

	if iWVSlavery > -1:

		iSlaveMarket = GC.getInfoTypeForString("BUILDING_SLAVE_MARKET")
		aiSlaveBuildings = [
			GC.getInfoTypeForString("BUILDING_SLAVERY"),
			GC.getInfoTypeForString("BUILDING_SLAVERY_BAD_ZORO_I"),
			GC.getInfoTypeForString("BUILDING_SLAVERY_BAD_ZORO_II"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_MILITARY_SUPPORT"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_ENTERTAINMENT"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_FOOD"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_PRODUCTION"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_COMMERCE"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_SANITATION"),
		]
		iSlaveFood = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD")
		iSlaveProd = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION")
		iSlaveHealth = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH")
		iSlaveEntertain = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT")

		iUnitFreedSlave = GC.getInfoTypeForString("UNIT_FREED_SLAVE")
		iUnitEntertain = GC.getInfoTypeForString("UNIT_STORY_TELLER")
		iUnitMerCaravan = GC.getInfoTypeForString("UNIT_EARLY_MERCHANT_C2C")
		#iUnitFoodMerchant = GC.getInfoTypeForString("UNIT_EARLY_FOOD_MERCHANT_C2C")
		iUnitHealth = GC.getInfoTypeForString("UNIT_HEALER")

		bMessage = iPlayer == GAME.getActivePlayer()
		if bMessage:
			msg = "Slavery worldview eradicated"
			CvUtil.sendMessage(msg, iPlayer, 16, unit.getButton(), ColorTypes(8), unit.getX(), unit.getY(), True, True, 0, "AS2D_DISCOVERBONUS")

		iCost = player.getBuildingProductionNeeded(iSlaveMarket)
		iSum = 0
		for city in player.cities():
			if bMessage:
				sCityName = city.getName()
			iCityX = city.getX()
			iCityY = city.getY()
			# Remove the main slavery building
			if city.getNumActiveBuilding(iWVSlavery) > 0:
				city.setNumRealBuilding(iWVSlavery, 0)

			# Sell the Slave market if one exists
			if city.getNumActiveBuilding(iSlaveMarket) > 0:

				city.setNumRealBuilding(iSlaveMarket, 0)

				iSum += iCost

				if bMessage:
					msg = TRNSLTR.getText("TXT_KEY_MSG_SLAVE_MARKET_SOLD", (sCityName,))
					CvUtil.sendMessage(msg, iPlayer, 16, GC.getBuildingInfo(iSlaveMarket).getButton(), ColorTypes(8), iCityX, iCityY, True, True, 0, "AS2D_BUILD_BANK")

			# Remove all other Slavery Buildings if they exist
			for ibuilding in aiSlaveBuildings:
				if city.getNumActiveBuilding(ibuilding) > 0:
					city.setNumRealBuilding(ibuilding, 0)

			iFreeSlaves = 0
			for i in xrange(GC.getNumSpecialistInfos()):
				if GC.getSpecialistInfo(i).isSlave():

					iCount = city.getFreeSpecialistCount(i)
					if iCount < 1: continue

					city.changeFreeSpecialistCount(i, -iCount)

					if i == iSlaveEntertain:
						for j in xrange(iCount):
							player.initUnit(iUnitEntertain, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
						if bMessage:
							msg = TRNSLTR.getText("TXT_KEY_MSG_FREED_SLAVES_AS", (sCityName, GC.getUnitInfo(iUnitEntertain).getDescription(), iCount))
							CvUtil.sendMessage(msg, iPlayer, 12, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

					elif i == iSlaveProd:
						for j in xrange(iCount):
							player.initUnit(iUnitMerCaravan, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
						if bMessage:
							msg = TRNSLTR.getText("TXT_KEY_MSG_FREED_SLAVES_AS", (sCityName, GC.getUnitInfo(iUnitMerCaravan).getDescription(), iCount))
							CvUtil.sendMessage(msg, iPlayer, 12, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

					elif i == iSlaveFood:
						for j in xrange(iCount):
							player.initUnit(iUnitMerCaravan, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
						if bMessage:
							msg = TRNSLTR.getText("TXT_KEY_MSG_FREED_SLAVES_AS", (sCityName, GC.getUnitInfo(iUnitMerCaravan).getDescription(), iCount))
							CvUtil.sendMessage(msg, iPlayer, 12, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

					elif i == iSlaveHealth:
						for j in xrange(iCount):
							player.initUnit(iUnitHealth, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
						if bMessage:
							msg = TRNSLTR.getText("TXT_KEY_MSG_FREED_SLAVES_AS", (sCityName, GC.getUnitInfo(iUnitHealth).getDescription(), iCount))
							CvUtil.sendMessage(msg, iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

					else: iFreeSlaves += iCount

			if iFreeSlaves > 0:
				for j in xrange(iFreeSlaves):
					player.initUnit(iUnitFreedSlave, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
				if bMessage:
					msg = TRNSLTR.getText("TXT_KEY_MSG_FREED_SLAVES_AS", (sCityName, GC.getUnitInfo(iUnitFreedSlave).getDescription(), iFreeSlaves))
					CvUtil.sendMessage(msg, iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

		if iSum > 0:
			player.changeGold(int(iSum * 0.2))

def doRemoveWVCannibalism(argsList):
	CyUnit = argsList[0]

	if CyUnit == None:
		print "[INFO] doRemoveWVCannibalism(CyUnit) where CyUnit is None"
		return # False call

	iType = GC.getInfoTypeForString("BUILDING_WV_CANNIBALISM")
	if iType > -1:
		iPlayer = CyUnit.getOwner()
		CyPlayer = GC.getPlayer(iPlayer)
		CyCity = CyPlayer.getCapitalCity()
		if CyCity is None:
			print "[INFO] doRemoveWVCannibalism(args) happened for a player with no cities"
		else:
			iType0 = GC.getInfoTypeForString("BUILDING_CANNIBALISM")
			for CyCity in CyPlayer.cities():
				CyCity.setNumRealBuilding(iType, 0)
				if iType0 > -1:
					CyCity.setNumRealBuilding(iType0, 0)

			if iPlayer == GC.getGame().getActivePlayer():
				CvUtil.sendImmediateMessage(TRNSLTR.getText("TXT_KEY_MSG_NO_CANNIBALISM", ()))
				CyAudioGame().Play2DSound("AS2D_DISCOVERBONUS")

def doRemoveWVHumanSacrifice(argsList):
	CyUnit = argsList[0]

	if CyUnit == None:
		return # False call

	iWVSacrifice = GC.getInfoTypeForString("BUILDING_WV_HUMAN_SACRIFICE")
	if iWVSacrifice > -1:
		iAltar = GC.getInfoTypeForString("BUILDING_HUMAN_SACRIFICE_ALTAR")
		iToken = GC.getInfoTypeForString("BUILDING_HUMAN_SACRIFICE")
		CyPlayer = GC.getPlayer(CyUnit.getOwner())

		for CyCity in CyPlayer.cities():
			# Remove the main worldview building
			if CyCity.getNumActiveBuilding(iWVSacrifice) > 0:
				CyCity.setNumRealBuilding(iWVSacrifice, 0)
				CyAudioGame().Play2DSound("AS2D_DISCOVERBONUS")

				CyInterface().addMessage(CyPlayer.getID(),False,25,TRNSLTR.getText("TXT_KEY_MSG_NO_HUMAN_SACRIFICE",(CyCity.getName(),)),"AS2D_BUILD_BANK",InterfaceMessageTypes.MESSAGE_TYPE_INFO,pUnit.getButton(),ColorTypes(8),CyCity.getX(),CyCity.getY(),True,True)

			# Remove the worldview token building
			CyCity.setNumRealBuilding(iToken, 0)

			# Remove the human sacrifice altar
			if CyCity.getNumActiveBuilding(iAltar) > 0:
				CyCity.setNumRealBuilding(iAltar, 0)

def getNumNonSpecialistSlaves(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesFood(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesProduction(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesCommerce(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesHealth(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesEntertainment(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesTutor(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesMilitary(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY"))

	return (iNormalSlaves - iSpecialSlaves)

def hasSufficientPopulation(argsList):
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	iNumSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY"))

	iPopulation = pCity.getPopulation()

	return (iNumSlaves < (10 * iPopulation))

def doAddSettledSlave(argsList):
	pUnit = argsList[0]
	print "caveman2Cosmos - doAddSettledSlave called."

	if pUnit == None:
		print "caveman2Cosmos - doAddSettledSlave called - not a valid unit."
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		print "caveman2Cosmos - doAddSettledSlave called - not a valid city."
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"),1)

def doAddSettledSlaveFood(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD"),1)

def doAddSettledSlaveProduction(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION"),1)

def doAddSettledSlaveCommerce(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE"),1)

def doAddSettledSlaveHealth(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH"),1)

def doAddSettledSlaveEntertainment(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT"),1)

def doAddSettledSlaveTutor(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR"),1)

def doAddSettledSlaveMilitary(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY"),1)

def doSacrificeCaptive(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pPlayer = GC.getPlayer(pUnit.getOwner())
	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	iGoldenAgeLength = 0
	iGoldenAgeLengthModifier = 1

	iUnitType = pUnit.getUnitType()
	if iUnitType == GC.getInfoTypeForString("UNIT_CAPTIVE_NEANDERTHAL"):
		iGoldenAgeLengthModifier = 0
	elif iUnitType == GC.getInfoTypeForString("UNIT_CAPTIVE_MILITARY"):
		iGoldenAgeLengthModifier = 1
	elif iUnitType == GC.getInfoTypeForString("UNIT_CAPTIVE_CIVILIAN"):
		iGoldenAgeLengthModifier = 2
	else:
		return # bad call

	if  pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_SACRIFICIAL_ALTAR")) > 0:
		iGoldenAgeLength += 1
	elif pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_HUMAN_SACRIFICE_ALTAR")) > 0:
		iGoldenAgeLength += 2

	iDruid = GC.getInfoTypeForString("RELIGION_DRUIDIC_TRADITIONS")
	iMeasoamerican = GC.getInfoTypeForString("RELIGION_NAGHUALISM")
	iStateReligion = pUnit.getStateReligion()

	if iStateReligion == iDruid :
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_SHRINE")) > 0:
			iGoldenAgeLengthModifier = iGoldenAgeLengthModifier + 1
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_CATHEDRAL2")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 3
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_CATHEDRAL")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 2
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_MONASTERY")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 1
	else:
		if pCity.isHasReligion(iDruid):
			if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_SHRINE")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_CATHEDRAL2")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_CATHEDRAL")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_MONASTERY")) > 0:
				iGoldenAgeLength = iGoldenAgeLength + 1

	if iStateReligion == iMeasoamerican :
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_SHRINE")) > 0:
			iGoldenAgeLengthModifier = iGoldenAgeLengthModifier + 1
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_CATHEDRAL")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 3
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_MONASTERY")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 2
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_TEMPLE")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 1
	else:
		if pCity.isHasReligion(iMeasoamerican):
			if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_SHRINE")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_CATHEDRAL")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_MONASTERY")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_TEMPLE")) > 0:
				iGoldenAgeLength = iGoldenAgeLength + 1

	iTotalGoldenAgeLength = iGoldenAgeLength * iGoldenAgeLengthModifier
	pPlayer.changeGoldenAgeTurns(iTotalGoldenAgeLength)


################ SPREAD RESOURCES ###################

def canBuildCowBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isFlatlands() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (GC.getFEATURE_FLOOD_PLAINS(), GC.getInfoTypeForString("FEATURE_SAVANNA"), -1): return 0
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS")): return 0
	return 1

def doBuildCowBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_COW"))

def canBuildCowBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isFlatlands() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_SWAMP"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() in (
		GC.getInfoTypeForString("TERRAIN_SALT_FLATS"),	GC.getInfoTypeForString("TERRAIN_DUNES"),		GC.getTERRAIN_DESERT(),
		GC.getInfoTypeForString("TERRAIN_TAIGA"),		GC.getInfoTypeForString("TERRAIN_ICE"),			GC.getInfoTypeForString("TERRAIN_TUNDRA"),
		GC.getInfoTypeForString("TERRAIN_PERMAFROST"),	GC.getInfoTypeForString("TERRAIN_JAGGED"),	GC.getInfoTypeForString("TERRAIN_BADLAND"),
		GC.getInfoTypeForString("TERRAIN_BARREN"), 		GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildCowBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return
	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_COW"))

def canBuildHorseBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isFlatlands() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (GC.getFEATURE_FLOOD_PLAINS(), GC.getInfoTypeForString("FEATURE_SAVANNA"), -1): return 0
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS")): return 0
	return 1

def doBuildHorseBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_HORSE"))

def canBuildHorseBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isFlatlands() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_SWAMP"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() in (
		GC.getInfoTypeForString("TERRAIN_SALT_FLATS"),	GC.getInfoTypeForString("TERRAIN_DUNES"),	GC.getTERRAIN_DESERT(),
		GC.getInfoTypeForString("TERRAIN_TAIGA"),		GC.getInfoTypeForString("TERRAIN_ICE"),		GC.getInfoTypeForString("TERRAIN_TUNDRA"),
		GC.getInfoTypeForString("TERRAIN_PERMAFROST"),	GC.getInfoTypeForString("TERRAIN_JAGGED"),	GC.getInfoTypeForString("TERRAIN_BADLAND"),
		GC.getInfoTypeForString("TERRAIN_BARREN"), 		GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildHorseBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_HORSE"))

def canBuildDonkeyBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (GC.getFEATURE_FLOOD_PLAINS(), GC.getInfoTypeForString("FEATURE_SAVANNA"), -1): return 0
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS"), GC.getInfoTypeForString("TERRAIN_SCRUB")): return 0
	return 1

def doBuildDonkeyBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_DONKEY"))

def canBuildDonkeyBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_SWAMP"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() in (
		GC.getInfoTypeForString("TERRAIN_SALT_FLATS"),	GC.getInfoTypeForString("TERRAIN_DUNES"),	GC.getTERRAIN_DESERT(),
		GC.getInfoTypeForString("TERRAIN_TAIGA"),		GC.getInfoTypeForString("TERRAIN_ICE"),		GC.getInfoTypeForString("TERRAIN_TUNDRA"),
		GC.getInfoTypeForString("TERRAIN_PERMAFROST"),	GC.getInfoTypeForString("TERRAIN_JAGGED"),	GC.getInfoTypeForString("TERRAIN_BADLAND"),
		GC.getInfoTypeForString("TERRAIN_BARREN"), 		GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildDonkeyBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_DONKEY"))

def canBuildSheepBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isHills() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (GC.getFEATURE_FLOOD_PLAINS(), GC.getInfoTypeForString("FEATURE_SAVANNA"), -1): return 0
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS")): return 0
	return 1

def doBuildSheepBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_SHEEP"))

def canBuildSheepBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isHills() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_SWAMP"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() in (
		GC.getInfoTypeForString("TERRAIN_SALT_FLATS"),	GC.getInfoTypeForString("TERRAIN_DUNES"),		GC.getTERRAIN_DESERT(),
		GC.getInfoTypeForString("TERRAIN_TAIGA"),		GC.getInfoTypeForString("TERRAIN_ICE"),			GC.getInfoTypeForString("TERRAIN_TUNDRA"),
		GC.getInfoTypeForString("TERRAIN_PERMAFROST"),	GC.getInfoTypeForString("TERRAIN_JAGGED"),	GC.getInfoTypeForString("TERRAIN_BADLAND"),
		GC.getInfoTypeForString("TERRAIN_BARREN"), 		GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildSheepBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_SHEEP"))

def canBuildCamelBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_DUNES"), GC.getTERRAIN_DESERT(), GC.getInfoTypeForString("TERRAIN_SCRUB")): return 0
	return 1

def doBuildCamelBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_CAMEL"))

def canBuildCamelBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_DUNES"), GC.getTERRAIN_DESERT(), GC.getInfoTypeForString("TERRAIN_SCRUB")): return 0
	return 1

def doBuildCamelBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_CAMEL"))

def canBuildLlamaBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getTerrainType() not in (
		GC.getInfoTypeForString("TERRAIN_BARREN"),	GC.getTERRAIN_DESERT(),	GC.getInfoTypeForString("TERRAIN_SCRUB"),
		GC.getInfoTypeForString("TERRAIN_ROCKEY"),	GC.getInfoTypeForString("TERRAIN_BADLAND")
		): return 0
	return 1

def doBuildLlamaBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_LLAMA"))

def canBuildLlamaBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getTerrainType() not in (
		GC.getInfoTypeForString("TERRAIN_BARREN"),	GC.getTERRAIN_DESERT(),	GC.getInfoTypeForString("TERRAIN_SCRUB"),
		GC.getInfoTypeForString("TERRAIN_ROCKEY"),	GC.getInfoTypeForString("TERRAIN_BADLAND")
		): return 0
	return 1

def doBuildLlamaBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_LLAMA"))

def canBuildPigBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (
		GC.getFEATURE_FOREST(), GC.getInfoTypeForString("FEATURE_FOREST_ANCIENT"),
		GC.getFEATURE_JUNGLE(), GC.getFEATURE_FLOOD_PLAINS()
		): return f
	if pPlot.getTerrainType() not in (
		GC.getInfoTypeForString("TERRAIN_SCRUB"), GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS"),
		GC.getInfoTypeForString("TERRAIN_LUSH"), GC.getInfoTypeForString("TERRAIN_MUDDY"), GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildPigBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_PIG"))

def canBuildPigBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_MANGROVE"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() not in (
		GC.getInfoTypeForString("TERRAIN_SCRUB"), GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS"),
		GC.getInfoTypeForString("TERRAIN_LUSH"), GC.getInfoTypeForString("TERRAIN_MUDDY"), GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildPigBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_PIG"))


################ MULTIMAPS ###################

def canGoToEarth(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_EARTH

def goToEarth(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_EARTH).moveUnitToMap(pUnit)

def canGoToSubterrain(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_SUBTERRAIN

def goToSubterrain(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_SUBTERRAIN).moveUnitToMap(pUnit)

def canGoToCislunarSpace(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_CISLUNAR

def goToCislunarSpace(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_CISLUNAR).moveUnitToMap(pUnit)

def canGoToMoon(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_MOON

def goToMoon(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_MOON).moveUnitToMap(pUnit)

def canGoToMars(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_MARS

def goToMars(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_MARS).moveUnitToMap(pUnit)

def canGoToVenus(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_VENUS

def goToVenus(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_VENUS).moveUnitToMap(pUnit)

def canGoToInnerSolarSystem(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_INNER_SOLAR_SYSTEM

def goToInnerSolarSystem(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_INNER_SOLAR_SYSTEM).moveUnitToMap(pUnit)

def canGoToOuterSolarSystem(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_OUTER_SOLAR_SYSTEM

def goToOuterSolarSystem(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_OUTER_SOLAR_SYSTEM).moveUnitToMap(pUnit)

def canGoToTitan(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_TITAN

def goToTitan(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_TITAN).moveUnitToMap(pUnit)

def canGoToTransneptunianSpace(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_TRANSNEPTUNIAN

def goToTransneptunianSpace(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_TRANSNEPTUNIAN).moveUnitToMap(pUnit)

def canGoToNearbyStars(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_NEARBY_STARS

def goToNearbyStars(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_NEARBY_STARS).moveUnitToMap(pUnit)

def canGoToOrionArm(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_ORION_ARM

def goToOrionArm(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_ORION_ARM).moveUnitToMap(pUnit)

def canGoToMilkyWay(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_MILKY_WAY

def goToMilkyWay(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_MILKY_WAY).moveUnitToMap(pUnit)

def canGoToLocalGroup(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_LOCAL_GROUP

def goToLocalGroup(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_LOCAL_GROUP).moveUnitToMap(pUnit)

def canGoToVirgoSupercluster(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_VIRGO_SUPERCLUSTER

def goToVirgoSupercluster(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_VIRGO_SUPERCLUSTER).moveUnitToMap(pUnit)

def canGoToUniverse(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_UNIVERSE

def goToUniverse(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_UNIVERSE).moveUnitToMap(pUnit)

def canGoToDistantCosmos(argsList):
	return GAME.getCurrentMap() != MapTypes.MAP_DISTANT_COSMOS

def goToDistantCosmos(argsList):
	pUnit = argsList[0]
	GC.getMapByIndex(MapTypes.MAP_DISTANT_COSMOS).moveUnitToMap(pUnit)
