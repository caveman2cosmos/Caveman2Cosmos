# # # # #
# Partisan Mod (by GIR)
#
### number of Partisan units depending on city size (before conquest) and if  ###
### the loser of the city is stronger or weaker (military power) as the new   ###
###                          owner of the city.                               ###
###---------------------------------------------------------------------------###
###  city size  | Partisan Units if | Partisan Units if   | reduce population ###
### before conq.| loser weaker conq.| loser stronger conq.|                   ###
###-------------+-------------------+---------------------+-------------------###
###    01-06    |       1 - 2       |       0 - 1         |         0         ###
###    07-13    |       2 - 3       |       1 - 2         |  - num partisans  ###
###    14-20    |       3 - 5       |       1 - 3         |  - num partisans  ###
###    21-27    |       4 - 6       |       2 - 4         |  - num partisans  ###
###    28-34    |       5 - 8       |       2 - 5         |  - num partisans  ###
###    35-41    |       6 - 9       |       3 - 6         |  - num partisans  ###
###     ...     |        ...        |        ...          |  - num partisans  ###
###-------------+-------------------+---------------------+-------------------###
###             |  +3 units with    |  +3 units with      |                   ###
###             |  NATIONHOOD civic |  NATIONHOOD civic   |                   ###
###-------------+-------------------+---------------------+-------------------###
###             |  +1 units with    |  +1 units with      |                   ###
###             |  PROTECTIVE trait |  PROTECTIVE trait   |                   ###


###      new created Partisan units will get more and more promotions with higher       ###
###      tech level and also get special promotions dependig on the "start" plot        ###
###-------------------------------------------------------------------------------------###
###     technology      |     plot types      |      promotions      |      chance      ###
###---------------------+---------------------+----------------------+------------------###
###      rifling        |          you need rifling tech to get Partisan units          ###
###    assembly line    |          -          |       + drill        |       100%       ###
###    industrialism    |          -          |       + drill        |       100%       ###
###      rocketry       |          -          |       + drill        |       100%       ###
###      plastics       |          -          |       + drill        |       100%       ###
###      railroad       |          -          |     + flanking I     |       100%       ###
###     combustion      |          -          |     + flanking II    |       100%       ###
###       radio         |          -          |       + morale       |        50%       ###
###      fascism        |          -          |       + combat       |        50%       ###
###      computers      |          -          |       + combat       |       100%       ###
###      robotics       |          -          |       + combat       |       100%       ###
###     composites      |          -          |       + combat       |       100%       ###
###       laser         |          -          |       + combat       |       100%       ###
###         -           |   forest / jungle   |     + woodsman I     |       100%       ###
###    assembly line    |   forest / jungle   |     + woodsman II    |        75%       ###
###    industrialism    |   forest / jungle   |     + woodsman III   |        50%       ###
###         -           |        hill         |     + guerilla I     |       100%       ###
###    assembly line    |        hill         |     + guerilla II    |        75%       ###
###    industrialism    |        hill         |     + guerilla III   |        50%       ###

from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

def onCityAcquired(argsList):
	iOwnerOld, iOwnerNew, CyCity, bConquest, bTrade = argsList

	# Partizans doesn't make sense if this is reconquest (liberation) or not conquest at all.
	if not bConquest or not CyCity.isOccupation():
		return

	CyPlayerOld = GC.getPlayer(iOwnerOld)
	if not CyPlayerOld.isAlive():
		return

	# Partisans only available with rifling tech
	CyTeamOld = GC.getTeam(CyPlayerOld.getTeam())
	if not CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_RIFLING")):
		return

################################################
### Get number of Partisan Units (nPartisan) ###
################################################

	# Get base nPartisan number
	iPop = CyCity.getPopulation()
	nPartisan = 1 + iPop / 5 # pop1 = 1; pop5 = 2; pop15 = 4; pop30 = 7; pop55 = 12

	iRandom = int(iPop ** 0.4) # pop1 = 1; pop6 = 2; pop16 = 3; pop32 = 4; pop56 = 5

	# iRandom must be 1 or more at this point.

	# Add/subtract the random number from the nPartisan (depending on power)
	if CyPlayerOld.getPower() > GC.getPlayer(iOwnerNew).getPower():
		nPartisan -= GAME.getSorenRandNum(1 + iRandom, "Random for less Partisans")
	else:
		nPartisan += GAME.getSorenRandNum(1 + iRandom, "Random for more Partisans")

	if nPartisan > 0:
		# Change base nPartisan number depending on the culture of the conqPlayer in the city
		# If the conqPlayer has 40% culture in the city, only create 60% of the number of the Partisans
		nPartisan = nPartisan * (100 - CyCity.plot().calculateCulturePercent(iOwnerNew)) / 100

	# +1 partisans with protective trait (only loserPlayer) ###
	if nPartisan > -1 and CyPlayerOld.hasTrait(GC.getInfoTypeForString('TRAIT_PROTECTIVE')):
		nPartisan += 1

	elif nPartisan < 1: return

##########################
### set Partisan Units ###
##########################

	ft_forest = GC.getFEATURE_FOREST()
	ft_jungle = GC.getFEATURE_JUNGLE()
	it_fort = GC.getInfoTypeForString("IMPROVEMENT_FORT")
	# Check all city radius plots
	iX = CyCity.getX()
	iY = CyCity.getY()
	firstRing = [(iX-1, iY-1), (iX-1, iY), (iX-1, iY-1), (iX, iY-1), (iX, iY+1), (iX+1, iY-1), (iX+1, iY), (iX+1, iY+1)]
	plots = []
	for iXLoop in range(iX - 3, iX + 4, 1):
		for iYLoop in range(iY - 3, iY + 4, 1):

			CyPlot = CyMap().plot(iXLoop, iYLoop)
			if CyPlot.isNone() or CyPlot.isCity() or CyPlot.isWater() or CyPlot.isImpassable():
				continue

			if not CyPlot.isVisibleEnemyUnit(iOwnerOld):
				plots.append(CyPlot)
				# Doubles the chance to set the partisan in the inner city radius
				if (iXLoop, iYLoop) in firstRing:
					plots.append(CyPlot)
					# Increase the chance to set the partisan in the inner city radius with certain extras
					if CyPlot.isHills() or CyPlot.getFeatureType() in (ft_forest, ft_jungle) or CyPlot.getImprovementType() == it_fort:
						for i in range(4):
							plots.append(CyPlot)
	# Set partisan(s)
	if not plots: return
	iPlots = len(plots)

########################################
### identify tech related promotions ###
########################################

	# Promotion counts and booleans
	drillpromotiontech = 0
	combatpromotiontech = 0

	bHasAssemblyLine = False
	bHasIndustrialism = False
	bHasRailroad = False
	bHasRadio = False
	bHasFascism = False

	PROMOTION_WOODSMAN1 = GC.getInfoTypeForString("PROMOTION_WOODSMAN1")
	PROMOTION_GUERILLA1 = GC.getInfoTypeForString("PROMOTION_GUERILLA1")

	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_ASSEMBLY_LINE")):
		bHasAssemblyLine = True
		drillpromotiontech += 1
		PROMOTION_WOODSMAN2 = GC.getInfoTypeForString("PROMOTION_WOODSMAN2")
		PROMOTION_GUERILLA2 = GC.getInfoTypeForString("PROMOTION_GUERILLA2")

	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_INDUSTRIALISM")):
		bHasIndustrialism = True
		drillpromotiontech += 1
		PROMOTION_WOODSMAN3 = GC.getInfoTypeForString("PROMOTION_WOODSMAN3")
		PROMOTION_GUERILLA3 = GC.getInfoTypeForString("PROMOTION_GUERILLA3")

	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_ROCKETRY")):
		drillpromotiontech += 1
	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_PLASTICS")):
		drillpromotiontech += 1

	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_RAILROAD")):
		bHasRailroad = True
		PROMOTION_FLANKING1 = GC.getInfoTypeForString('PROMOTION_FLANKING1')
		bHasCombustion = False

		if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_COMBUSTION")):
			bHasCombustion = True
			PROMOTION_FLANKING2 = GC.getInfoTypeForString('PROMOTION_FLANKING2')

	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_RADIO")):
		bHasRadio = True
		PROMOTION_MORALE = GC.getInfoTypeForString('PROMOTION_MORALE')

	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_FASCISM")):
		bHasFascism = True
	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_PERSONAL_COMPUTERS")):
		combatpromotiontech += 1
	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_ROBOTICS")):
		combatpromotiontech += 1
	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_COMPOSITES")):
		combatpromotiontech += 1
	if CyTeamOld.isHasTech(GC.getInfoTypeForString("TECH_LASER")):
		combatpromotiontech += 1

	if combatpromotiontech or bHasFascism:
		p_combat1 = GC.getInfoTypeForString("PROMOTION_COMBAT1")
		p_combat2 = GC.getInfoTypeForString("PROMOTION_COMBAT2")
		p_combat3 = GC.getInfoTypeForString("PROMOTION_COMBAT3")
		p_combat4 = GC.getInfoTypeForString("PROMOTION_COMBAT4")
		p_combat5 = GC.getInfoTypeForString("PROMOTION_COMBAT5")
		p_combat6 = GC.getInfoTypeForString("PROMOTION_COMBAT6")

	if drillpromotiontech:
		PROMOTION_DRILL1 = GC.getInfoTypeForString("PROMOTION_DRILL1")
		PROMOTION_DRILL2 = GC.getInfoTypeForString("PROMOTION_DRILL2")
		PROMOTION_DRILL3 = GC.getInfoTypeForString("PROMOTION_DRILL3")
		PROMOTION_DRILL4 = GC.getInfoTypeForString("PROMOTION_DRILL4")

	# Message related
	iPlayerAct = GAME.getActivePlayer()
	if iPlayerAct in (iOwnerNew, iOwnerOld):
		szName = CyCity.getName()

	# Start partisan generation
	UNIT_PARTISAN = GC.getInfoTypeForString('UNIT_PARTISAN')
	iCount = nPartisan
	iDamage = 0

	while iCount > 0:
		iCount -= 1
		CyPlot = plots[GAME.getSorenRandNum(iPlots, "Random CyPlot for Partisan")]
		iiX = CyPlot.getX()
		iiY = CyPlot.getY()
		pNewUnit = CyPlayerOld.initUnit(UNIT_PARTISAN, iiX, iiY, UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.NO_DIRECTION )

################################
### set aditional promotions ###
################################

	# Tech lvl depending promotions

		# DRILL promotions
		iDrillCount = drillpromotiontech
		while iDrillCount > 0:
			iDrillCount -= 1
			if not pNewUnit.isHasPromotion(PROMOTION_DRILL1):
				pNewUnit.setHasPromotion(PROMOTION_DRILL1, True)
			elif not pNewUnit.isHasPromotion(PROMOTION_DRILL2):
				pNewUnit.setHasPromotion(PROMOTION_DRILL2, True)
			elif not pNewUnit.isHasPromotion(PROMOTION_DRILL3):
				pNewUnit.setHasPromotion(PROMOTION_DRILL3, True)
			elif not pNewUnit.isHasPromotion(PROMOTION_DRILL4):
				pNewUnit.setHasPromotion(PROMOTION_DRILL4, True)

		if bHasRailroad:
			pNewUnit.setHasPromotion(PROMOTION_FLANKING1, True)
			if bHasCombustion:
				pNewUnit.setHasPromotion(PROMOTION_FLANKING2, True)
		if bHasRadio:
			if GAME.getSorenRandNum( 2, "Random Morale"):
				pNewUnit.setHasPromotion(PROMOTION_MORALE, True)

		iCombatCount = combatpromotiontech
		if bHasFascism:
			# 50% chance
			if GAME.getSorenRandNum( 2, "Random Combat1/2"):
				iCombatCount += 1
		while iCombatCount:
			iCombatCount -= 1
			if not pNewUnit.isHasPromotion(p_combat1):
				pNewUnit.setHasPromotion(p_combat1, True)
			elif not pNewUnit.isHasPromotion(p_combat2):
				pNewUnit.setHasPromotion(p_combat2, True)
			elif not pNewUnit.isHasPromotion(p_combat3):
				pNewUnit.setHasPromotion(p_combat3, True)
			elif not pNewUnit.isHasPromotion(p_combat4):
				pNewUnit.setHasPromotion(p_combat4, True)
			elif not pNewUnit.isHasPromotion(p_combat5):
				pNewUnit.setHasPromotion(p_combat5, True)
			elif not pNewUnit.isHasPromotion(p_combat6):
				pNewUnit.setHasPromotion(p_combat6, True)

	# plot type depending promotions

		# WOODSMAN promotion
		if CyPlot.getFeatureType() in (ft_forest, ft_jungle):
			pNewUnit.setHasPromotion(PROMOTION_WOODSMAN1, True)
			if bHasAssemblyLine:
				# 75% chance
				if GAME.getSorenRandNum(4, "Random Woodsman2"):
					pNewUnit.setHasPromotion(PROMOTION_WOODSMAN2, True)
					if bHasIndustrialism:
						# 75% chance, effectively 56%.
						if GAME.getSorenRandNum(4, "Random Woodsman3"):
							pNewUnit.setHasPromotion(PROMOTION_WOODSMAN3, True)

		# GUERILLA promotion
		if CyPlot.isHills():
			pNewUnit.setHasPromotion(PROMOTION_GUERILLA1, True)
			if bHasAssemblyLine:
				# 75% chance
				if GAME.getSorenRandNum(4, "Random Guerilla2"):
					pNewUnit.setHasPromotion(PROMOTION_GUERILLA2, True)
					if bHasIndustrialism:
						# 75% chance, effectively 56%.
						if GAME.getSorenRandNum(4, "Random Guerilla3"):
							pNewUnit.setHasPromotion(PROMOTION_GUERILLA3, True)

###########################################
### Random damage to nearby enemy units ###
###########################################

		# Find all EnemyUnits near the partisan
		lEnemyUnits = []
		for CyPlotX in CyCity.plot().rect(1, 1):
			if CyPlotX.isVisibleEnemyUnit(iOwnerOld):
				for CyUnitX in CyPlotX.units():
					lEnemyUnits.append(CyUnitX)

		if lEnemyUnits:

			# Random number between 1 and 5 (but not more than EnemyUnits near the partisan unit)
			n_EnemyUnits = len(lEnemyUnits)
			if n_EnemyUnits > 5:
				n_EnemyUnits = 5
			if n_EnemyUnits > 1:
				n_EnemyUnits = 1 + GAME.getSorenRandNum(n_EnemyUnits, "Random for how many eunits will suffer damage")

			while n_EnemyUnits:
				n_EnemyUnits -= 1
				# Choose which EnemyUnit will suffer damage from partisan unit
				ppUnit = lEnemyUnits[GAME.getSorenRandNum(len(lEnemyUnits), "Random which eunit will suffer damage")]
				# Random for how many damage the eunit will suffer from the partisan attack (15 - 30)
				iRand = GAME.getSorenRandNum(16, "rand damage") + 15

				# Check to not kill the unit
				iMaxDamage = ppUnit.getHP() - 1
				if iMaxDamage:
					if iRand > iMaxDamage:
						iRand = iMaxDamage

					ppUnit.changeDamage(iRand, 0)
					CyEngine().triggerEffect(GC.getInfoTypeForString("EFFECT_EXPLOSION_CITY"), ppUnit.plot().getPoint())
					iDamage += iRand

					if iPlayerAct == iOwnerNew:
						CvUtil.sendMessage("", iOwnerNew, -1, 'Art/Interface/Buttons/actions/destroy.dds', ColorTypes(7), ppUnit.getX(), ppUnit.getY(), True, True)
					elif iPlayerAct == iOwnerOld:
						CvUtil.sendMessage("", iOwnerOld, -1, 'Art/Interface/Buttons/actions/destroy.dds', ColorTypes(11), ppUnit.getX(), ppUnit.getY(), True, True)

		if iPlayerAct == iOwnerNew:
			CvUtil.sendMessage("", iOwnerNew, -1, 'Art/Interface/Buttons/Units/sparth/guerilla.dds', ColorTypes(7), iiX, iiY, True, True, 1, 'AS2D_CITY_REVOLT', False)
		elif iPlayerAct == iOwnerOld:
			CvUtil.sendMessage("", iOwnerOld, -1, 'Art/Interface/Buttons/Units/sparth/guerilla.dds', ColorTypes(11), iiX, iiY, True, True, 1, 'AS2D_CITY_REVOLT', False)

	if iDamage < 1:
		if iPlayerAct == iOwnerNew:
			CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_PARTISAN_GAMETXT4",(szName,)), iOwnerNew, 16, 'Art/Interface/Buttons/civics/despotism.dds', ColorTypes(7), iX, iY, True, True)
		elif iPlayerAct == iOwnerOld:
			CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_PARTISAN_GAMETXT3",(szName,)), iOwnerOld, 16, 'Art/Interface/Buttons/civics/despotism.dds', ColorTypes(44), iX, iY, True, True)

	elif iPlayerAct == iOwnerNew:
		CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_PARTISAN_GAMETXT2",(szName, iDamage)), iOwnerNew, 16, 'Art/Interface/Buttons/civics/despotism.dds', ColorTypes(7), iX, iY, True, True)
	elif iPlayerAct == iOwnerOld:
		CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_PARTISAN_GAMETXT1",(szName, iDamage)), iOwnerOld, 16, 'Art/Interface/Buttons/civics/despotism.dds', ColorTypes(44), iX, iY, True, True)

	# Reduce population
	if nPartisan > 1:
		nPartisan -= 1
		if iPop > nPartisan:
			CyCity.changePopulation(-nPartisan)
		elif iPop > 1: CyCity.changePopulation(1 - iPop)


# Partisan War Prize
def onCombatResult(argsList):
	##  First we check that the winning unit is a patisan and the loosing a seige or "armour" unit
	##  There is a small chance that the unit will be captured.
	CyUnitW, CyUnitL = argsList

	if CyUnitW.getUnitType() == GC.getInfoTypeForString('UNIT_PARTISAN'):
		captureChance = None
		iCombatL = CyUnitL.getUnitCombatType()
		if iCombatL == GC.getInfoTypeForString('UNITCOMBAT_SIEGE'):
			captureChance = 10
		elif iCombatL in (GC.getInfoTypeForString('UNITCOMBAT_TRACKED'), GC.getInfoTypeForString('UNITCOMBAT_WHEELED')):
			captureChance = 5
		if captureChance and GAME.getSorenRandNum(100, "Partisan capture unit") < captureChance:

			iPlayerW = CyUnitW.getOwner()
			CyPlayerW = GC.getPlayer(iPlayerW)

			iUnitL = CyUnitL.getUnitType()
			iX = CyUnitW.getX()
			iY = CyUnitW.getY()
			CyUnit = CyPlayerW.initUnit(iUnitL, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_NORTH)
			CyUnit.setDamage(75, False)
			CyUnit.finishMoves()

			iPlayerAct = GAME.getActivePlayer()
			if iPlayerAct == iPlayerW:
				CvUtil.sendMessage(
					TRNSLTR.getText("TXT_KEY_PARTISAN_CAPTURE_UNIT2", (GC.getUnitInfo(iUnitL).getDescription(),)),
					iPlayerAct, 16, 'Art/Interface/Buttons/civics/despotism.dds', ColorTypes(7), iX, iY, True, True
				)
			elif iPlayerAct == CyUnitL.getOwner():
				CvUtil.sendMessage(
					TRNSLTR.getText("TXT_KEY_PARTISAN_CAPTURE_UNIT1", (GC.getUnitInfo(iUnitL).getDescription(),)),
					iPlayerAct, 16, 'Art/Interface/Buttons/civics/despotism.dds', ColorTypes(44), iX, iY, True, True
				)