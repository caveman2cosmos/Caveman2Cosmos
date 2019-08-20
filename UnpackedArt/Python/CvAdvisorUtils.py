##
## CvAdvisorUtils

##  - Unoficial Patch Added
##  - Platyping's efficency code modifications added



from CvPythonExtensions import *

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

g_iAdvisorNags = 0
g_listNoLiberateCities = []
lPopulation = [	[2000000000, FeatTypes.FEAT_POPULATION_2_BILLION, "TXT_KEY_FEAT_2_BILLION"],
		[1000000000, FeatTypes.FEAT_POPULATION_1_BILLION, "TXT_KEY_FEAT_1_BILLION"],
		[500000000, FeatTypes.FEAT_POPULATION_500_MILLION, "TXT_KEY_FEAT_500_MILLION"],
		[200000000, FeatTypes.FEAT_POPULATION_200_MILLION, "TXT_KEY_FEAT_200_MILLION"],
		[100000000, FeatTypes.FEAT_POPULATION_100_MILLION, "TXT_KEY_FEAT_100_MILLION"],
		[50000000, FeatTypes.FEAT_POPULATION_50_MILLION, "TXT_KEY_FEAT_50_MILLION"],
		[20000000, FeatTypes.FEAT_POPULATION_20_MILLION, "TXT_KEY_FEAT_20_MILLION"],
		[10000000, FeatTypes.FEAT_POPULATION_10_MILLION, "TXT_KEY_FEAT_10_MILLION"],
		[5000000, FeatTypes.FEAT_POPULATION_5_MILLION, "TXT_KEY_FEAT_5_MILLION"],
		[2000000, FeatTypes.FEAT_POPULATION_2_MILLION, "TXT_KEY_FEAT_2_MILLION"],
		[1000000, FeatTypes.FEAT_POPULATION_1_MILLION, "TXT_KEY_FEAT_1_MILLION"],
		[500000, FeatTypes.FEAT_POPULATION_HALF_MILLION, "TXT_KEY_FEAT_HALF_MILLION"]]
lCorporations = []
lBonus = []
lUnitCombat = []

def featPopup(iPlayer):
	if not GC.getPlayer(iPlayer).isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
		return False
	if not GC.getPlayer(iPlayer).isHuman():
		return False
	if GAME.isNetworkMultiPlayer():
		return False
	if GAME.getElapsedGameTurns() == 0:
		return False
	return True


def resetAdvisorNags():
	global g_iAdvisorNags
	g_iAdvisorNags = 0

def resetNoLiberateCities():
	global g_listNoLiberateCities
	g_listNoLiberateCities = []

	global lCorporations
	lCorporations = []
	for iI in xrange(GC.getNumBuildingInfos()):
		Info = GC.getBuildingInfo(iI)
		eCorporation = Info.getFoundsCorporation()
		if eCorporation > -1 and not GAME.isCorporationFounded(eCorporation):
			lTechs = []
			iTech = Info.getPrereqAndTech()
			if iTech > -1:
				lTechs.append(iTech)
			for iPrereq in xrange(GC.getDefineINT("NUM_BUILDING_AND_TECH_PREREQS")):
				iTech = Info.getPrereqAndTechs(iPrereq)
				if iTech > -1:
					lTechs.append(iTech)

			iUnit = -1
			for i in xrange(GC.getNumUnitInfos()):
				if GC.getUnitInfo(i).getHasBuilding(iI):
					iUnit = i
					break
			if iUnit == -1: continue

			lTemp = []
			for iPrereq in xrange(GC.getDefineINT("NUM_CORPORATION_PREREQ_BONUSES")):
				eBonus = GC.getCorporationInfo(eCorporation).getPrereqBonus(iPrereq)
				if eBonus > -1:
					lTemp.append(eBonus)
			if len(lTemp) == 0: continue

			lCorporations.append([eCorporation, lTechs, iUnit, lTemp])
	global lBonus
	lBonus = []
	lLuxury = []
	lFood = []
	for i in xrange(GC.getNumBonusInfos()):
		if GC.getBonusInfo(i).getHappiness() > 0:
			lLuxury.append(i)
		if GC.getBonusInfo(i).getHealth() > 0:
			lFood.append(i)
	iBonus = GC.getInfoTypeForString("BONUS_COPPER")
	if iBonus > -1:
		lBonus.append([FeatTypes.FEAT_COPPER_CONNECTED, [iBonus], "TXT_KEY_FEAT_COPPER_CONNECTED"])
	iBonus = GC.getInfoTypeForString("BONUS_HORSE")
	if iBonus > -1:
		lBonus.append([FeatTypes.FEAT_HORSE_CONNECTED, [iBonus], "TXT_KEY_FEAT_HORSE_CONNECTED"])
	iBonus = GC.getInfoTypeForString("BONUS_IRON")
	if iBonus > -1:
		lBonus.append([FeatTypes.FEAT_IRON_CONNECTED, [iBonus], "TXT_KEY_FEAT_IRON_CONNECTED"])
	if len(lLuxury) > 0:
		lBonus.append([FeatTypes.FEAT_LUXURY_CONNECTED, lLuxury, "TXT_KEY_FEAT_LUXURY_CONNECTED"])
	if len(lFood) > 0:
		lBonus.append([FeatTypes.FEAT_FOOD_CONNECTED, lFood, "TXT_KEY_FEAT_FOOD_CONNECTED"])

	global lUnitCombat
	lUnitCombat = []
	for i in xrange(GC.getNumUnitCombatInfos()):
		lUnitCombat.append([-1, ""])

	iCombat = GC.getInfoTypeForString("UNITCOMBAT_ARCHER")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_ARCHER, "TXT_KEY_FEAT_UNITCOMBAT_ARCHER"]
	iCombat = GC.getInfoTypeForString("UNITCOMBAT_MOUNTED")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_MOUNTED, "TXT_KEY_FEAT_UNITCOMBAT_MOUNTED"]
	iCombat = GC.getInfoTypeForString("UNITCOMBAT_MELEE")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_MELEE, "TXT_KEY_FEAT_UNITCOMBAT_MELEE"]
	iCombat = GC.getInfoTypeForString("UNITCOMBAT_SIEGE")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_SIEGE, "TXT_KEY_FEAT_UNITCOMBAT_SIEGE"]
	iCombat = GC.getInfoTypeForString("UNITCOMBAT_GUN")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_GUN, "TXT_KEY_FEAT_UNITCOMBAT_GUN"]
	iCombat = GC.getInfoTypeForString("UNITCOMBAT_ARMOR")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_ARMOR, "TXT_KEY_FEAT_UNITCOMBAT_ARMOR"]
	iCombat = GC.getInfoTypeForString("UNITCOMBAT_HELICOPTER")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_HELICOPTER, "TXT_KEY_FEAT_UNITCOMBAT_HELICOPTER"]
	iCombat = GC.getInfoTypeForString("UNITCOMBAT_NAVAL")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_NAVAL, "TXT_KEY_FEAT_UNITCOMBAT_NAVAL"]


def unitBuiltFeats(pCity, pUnit):
	iCombat = pUnit.getUnitCombatType()
	iPlayer = pCity.getOwner()
	pPlayer = GC.getPlayer(iPlayer)
	if iCombat > -1:
		iFeat = lUnitCombat[iCombat][0]
		if iFeat > -1:
			if not pPlayer.isFeatAccomplished(lUnitCombat[iCombat][0]):
				pPlayer.setFeatAccomplished(lUnitCombat[iCombat][0], True)
				if featPopup(iPlayer) and GAME.getStartYear() == GC.getDefineINT("START_YEAR"):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
					popupInfo.setData1(lUnitCombat[iCombat][0])
					popupInfo.setData2(pCity.getID())
					popupInfo.setText(TRNSLTR.getText(lUnitCombat[iCombat][1], (pUnit.getNameKey(), pCity.getNameKey(),)))
					popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
					popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
					popupInfo.addPopup(iPlayer)

	if not pPlayer.isFeatAccomplished(FeatTypes.FEAT_UNIT_PRIVATEER):
		if GC.getUnitInfo(pUnit.getUnitType()).isHiddenNationality() and pUnit.getDomainType() == DomainTypes.DOMAIN_SEA:
			pPlayer.setFeatAccomplished(FeatTypes.FEAT_UNIT_PRIVATEER, True)
			if featPopup(iPlayer) and GAME.getStartYear() == GC.getDefineINT("START_YEAR"):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(FeatTypes.FEAT_UNIT_PRIVATEER)
				popupInfo.setData2(pCity.getID())
				popupInfo.setText(TRNSLTR.getText("TXT_KEY_FEAT_UNIT_PRIVATEER", (pUnit.getNameKey(), pCity.getNameKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(iPlayer)

	if not pPlayer.isFeatAccomplished(FeatTypes.FEAT_UNIT_SPY):
		if GC.getUnitInfo(pUnit.getUnitType()).isSpy():
			pPlayer.setFeatAccomplished(FeatTypes.FEAT_UNIT_SPY, True)
			if featPopup(iPlayer) and GAME.getStartYear() == GC.getDefineINT("START_YEAR"):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(FeatTypes.FEAT_UNIT_SPY)
				popupInfo.setData2(pCity.getID())
				popupInfo.setText(TRNSLTR.getText("TXT_KEY_FEAT_UNIT_SPY", (pUnit.getNameKey(), pCity.getNameKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(iPlayer)

def buildingBuiltFeats(pCity, iBuildingType):
	if not GC.getPlayer(pCity.getOwner()).isFeatAccomplished(FeatTypes.FEAT_NATIONAL_WONDER):
		if isNationalWonderClass(GC.getBuildingInfo(iBuildingType).getBuildingClassType()):
			GC.getPlayer(pCity.getOwner()).setFeatAccomplished(FeatTypes.FEAT_NATIONAL_WONDER, True)

			if (featPopup(pCity.getOwner()) and (GAME.getStartYear() == GC.getDefineINT("START_YEAR"))):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(FeatTypes.FEAT_NATIONAL_WONDER)
				popupInfo.setData2(pCity.getID())
				popupInfo.setText(TRNSLTR.getText("TXT_KEY_FEAT_NATIONAL_WONDER", (GC.getBuildingInfo(iBuildingType).getTextKey(), pCity.getNameKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(pCity.getOwner())

def populationFeat(iPlayer, eFeat, szText):
	GC.getPlayer(iPlayer).setFeatAccomplished(eFeat, True)
	if featPopup(iPlayer):
		popupInfo = CyPopupInfo()
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
		popupInfo.setData1(eFeat)
		popupInfo.setText(TRNSLTR.getText(szText, (GC.getPlayer(iPlayer).getCivilizationDescriptionKey(), )))
		popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
		popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
		popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
		popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
		popupInfo.addPopup(iPlayer)


def endTurnFeats(iPlayer):
	global g_listPopulationFeats
	lRealPopulation = GC.getPlayer(iPlayer).getRealPopulation()
	pPlayer = GC.getPlayer(iPlayer)
	for item in lPopulation:
		if pPlayer.isFeatAccomplished(item[1]): break
		if lRealPopulation > item[0]:
			populationFeat(iPlayer, item[1], item[2])

	pCapitalCity = pPlayer.getCapitalCity()
	if pCapitalCity.isNone(): return

	if not pPlayer.isFeatAccomplished(FeatTypes.FEAT_TRADE_ROUTE):
		(pCity, iter) = pPlayer.firstCity(False)
		while(pCity):
			if not pCity.isCapital():
				if pCity.isConnectedToCapital(iPlayer):
					pPlayer.setFeatAccomplished(FeatTypes.FEAT_TRADE_ROUTE, True)
					if featPopup(iPlayer) and GAME.getStartYear() == GC.getDefineINT("START_YEAR"):
						popupInfo = CyPopupInfo()
						popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
						popupInfo.setData1(FeatTypes.FEAT_TRADE_ROUTE)
						popupInfo.setData2(pCity.getID())
						popupInfo.setText(TRNSLTR.getText("TXT_KEY_FEAT_TRADE_ROUTE", (pCity.getNameKey(), )))
						popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
						popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
						popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
						popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
						popupInfo.addPopup(iPlayer)

					break
			(pCity, iter) = pPlayer.nextCity(iter, False)

	for item in lBonus:
		if pPlayer.isFeatAccomplished(item[0]): continue
		for iBonus in item[1]:
			if pCapitalCity.hasBonus(iBonus):
				pPlayer.setFeatAccomplished(item[0], True)
				if featPopup(iPlayer) and GAME.getStartYear() == GC.getDefineINT("START_YEAR"):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
					popupInfo.setData1(item[0])
					popupInfo.setData2(pCapitalCity.getID())
					popupInfo.setText(TRNSLTR.getText(item[2], (GC.getBonusInfo(iBonus).getTextKey(),)))
					popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
					popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
					popupInfo.addPopup(iPlayer)
					break

	if not pPlayer.isFeatAccomplished(FeatTypes.FEAT_CORPORATION_ENABLED):
		global lCorporations
		eTeam = pPlayer.getTeam()
		pTeam = GC.getTeam(eTeam)
		i = 0
		while i < len(lCorporations):
			item = lCorporations[i]
			if GAME.isCorporationFounded(item[0]):
				del lCorporations[i]
			else:
				bValid = True
				for iTech in item[1]:
					if not pTeam.isHasTech(iTech):
						bValid = False
						break
				if bValid:
					pPlayer.setFeatAccomplished(FeatTypes.FEAT_CORPORATION_ENABLED, True)
					szBonusList = u""
					for j in xrange(len(item[3])):
						eBonus = item[3][j]
						szBonusList += GC.getBonusInfo(eBonus).getDescription()
						if j != len(item[3]) - 1:
							szBonusList += TRNSLTR.getText("TXT_KEY_OR", ())

					szFounder = GC.getUnitInfo(item[2]).getTextKey()

					if featPopup(iPlayer) and GAME.getStartYear() == GC.getDefineINT("START_YEAR"):
						popupInfo = CyPopupInfo()
						popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
						popupInfo.setData1(FeatTypes.FEAT_CORPORATION_ENABLED)
						popupInfo.setData2(pCapitalCity.getID())
						popupInfo.setText(TRNSLTR.getText("TXT_KEY_FEAT_CORPORATION_ENABLED", (item[0], szFounder, szBonusList)))
						popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
						popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
						popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
						popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
						popupInfo.addPopup(iPlayer)
					break
				i += 1

def cityAdvise(CyCity, iPlayer):

	global g_iAdvisorNags

	if g_iAdvisorNags >= 2: return
	if CyCity.isDisorder(): return

	CyPlayer = GC.getPlayer(iPlayer)

	if CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS) and CyPlayer.isHuman() and not GAME.isNetworkMultiPlayer():

		iTurn = GAME.getGameTurn()
		iTurnFounded = CyCity.getGameTurnFounded()
		if iTurn % 40 == iTurnFounded % 40:

			iCityID = CyCity.getID()
			if not iCityID in g_listNoLiberateCities:
				iPlayerX = CyCity.getLiberationPlayer(False)
				if iPlayerX != -1:
					CyPlayerX = GC.getPlayer(iPlayerX)

					if GC.getTeam(CyPlayer.getTeam()).isHasMet(CyPlayerX.getTeam()):
						if not GC.getTeam(CyPlayerX.getTeam()).isAtWar(GAME.getActiveTeam()):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(iCityID)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_LIBERATION_DEMAND", (CyCity.getNameKey(), CyPlayerX.getCivilizationDescriptionKey(), CyPlayerX.getNameKey())))
							popupInfo.setOnClickedPythonCallback("liberateOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_listNoLiberateCities.append(iCityID)
							g_iAdvisorNags += 1

				elif CyPlayer.canSplitEmpire() and CyPlayer.canSplitArea(CyCity.area().getID()) and CyCity.AI_cityValue() < 0:
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
					popupInfo.setData1(iCityID)
					popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_COLONY_DEMAND", (CyCity.getNameKey(), )))
					popupInfo.setOnClickedPythonCallback("colonyOnClickedCallback")
					popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
					popupInfo.addPopup(iPlayer)
					g_listNoLiberateCities.append(iCityID)
					g_iAdvisorNags += 1

		if CyCity.isProduction():

			if not CyCity.isProductionUnit() and CyCity.getOrderQueueLength() <= 1:

				if (iTurn + 3) % 40 == iTurnFounded % 40:

					if GAME.getElapsedGameTurns() < 200 and CyCity.getPopulation() > 2 and not CyPlayer.AI_isFinancialTrouble():

						CyArea = CyCity.area()
						if not CyPlayer.AI_totalAreaUnitAIs(CyArea, UnitAITypes.UNITAI_SETTLE) and CyArea.getBestFoundValue(iPlayer) > 0:

							iBestValue = 0
							eBestUnit = -1

							for iI in xrange(GC.getNumUnitClassInfos()):

								if isLimitedUnitClass(iI): continue

								eLoopUnit = GC.getCivilizationInfo(CyPlayer.getCivilizationType()).getCivilizationUnits(iI)
								if eLoopUnit == -1: continue

								if GC.getUnitInfo(eLoopUnit).getDomainType() == DomainTypes.DOMAIN_LAND:

									if CyCity.canTrain(eLoopUnit, False, False, False, False):

										if CyCity.getFirstUnitOrder(eLoopUnit) == -1:

											iValue = CyPlayer.AI_unitValue(eLoopUnit, UnitAITypes.UNITAI_SETTLE, CyArea)

											if iValue > iBestValue:

												iBestValue = iValue
												eBestUnit = eLoopUnit

							if eBestUnit > -1:
								popupInfo = CyPopupInfo()
								popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
								popupInfo.setData1(CyCity.getID())
								popupInfo.setData2(OrderTypes.ORDER_TRAIN)
								popupInfo.setData3(eBestUnit)
								popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_UNIT_SETTLE_DEMAND", (GC.getUnitInfo(eBestUnit).getTextKey(), )))
								popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
								popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
								popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
								popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
								popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
								popupInfo.addPopup(iPlayer)
								g_iAdvisorNags += 1

				if (iTurn + 15) % 40 == iTurnFounded % 40:

					if CyCity.getPopulation() > 1 and not CyCity.countNumImprovedPlots():

						CyArea = CyCity.area()
						if CyCity.AI_countBestBuilds(CyArea) > 3:

							iBestValue = 0
							eBestUnit = -1

							for iI in xrange(GC.getNumUnitClassInfos()):

								if isLimitedUnitClass(iI): continue

								eLoopUnit = GC.getCivilizationInfo(CyPlayer.getCivilizationType()).getCivilizationUnits(iI)
								if eLoopUnit == -1: continue

								if GC.getUnitInfo(eLoopUnit).getDomainType() == DomainTypes.DOMAIN_LAND:

									if CyCity.canTrain(eLoopUnit, False, False, False, False):

										if CyCity.getFirstUnitOrder(eLoopUnit) == -1:

											iValue = CyPlayer.AI_unitValue(eLoopUnit, UnitAITypes.UNITAI_WORKER, CyArea)

											if iValue > iBestValue:

												iBestValue = iValue
												eBestUnit = eLoopUnit

						if eBestUnit != -1:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_TRAIN)
							popupInfo.setData3(eBestUnit)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_UNIT_WORKER_DEMAND", (CyCity.getNameKey(), GC.getUnitInfo(eBestUnit).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (iTurn + 27) % 40 == iTurnFounded % 40:

					if not CyCity.plot().getNumDefenders(iPlayer):

						CyArea = CyCity.area()
						iBestValue = 0
						eBestUnit = -1

						for iI in xrange(GC.getNumUnitClassInfos()):

							if isLimitedUnitClass(iI): continue

							eLoopUnit = GC.getCivilizationInfo(CyPlayer.getCivilizationType()).getCivilizationUnits(iI)
							if eLoopUnit == -1: continue

							if GC.getUnitInfo(eLoopUnit).getDomainType() == DomainTypes.DOMAIN_LAND:

								if CyCity.canTrain(eLoopUnit, False, False, False, False):

									iValue = CyPlayer.AI_unitValue(eLoopUnit, UnitAITypes.UNITAI_CITY_DEFENSE, CyArea) * 2
									iValue += CyPlayer.AI_unitValue(eLoopUnit, UnitAITypes.UNITAI_ATTACK, CyArea)

									if iValue > iBestValue:

										iBestValue = iValue
										eBestUnit = eLoopUnit

						if eBestUnit != -1:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_TRAIN)
							popupInfo.setData3(eBestUnit)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_UNIT_DEFENSE_DEMAND", (CyCity.getNameKey(), GC.getUnitInfo(eBestUnit).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (iTurn + 36) % 40 == iTurnFounded % 40:

					CyArea = CyCity.area()
					if not CyPlayer.AI_totalAreaUnitAIs(CyArea, UnitAITypes.UNITAI_MISSIONARY) and not GC.getTeam(CyPlayer.getTeam()).getAtWarCount(True):

						eStateReligion = CyPlayer.getStateReligion()

						if eStateReligion != -1:

							if CyPlayer.getHasReligionCount(eStateReligion) < CyPlayer.getNumCities() / 2:

								iBestValue = 0
								iBestUnit = -1
								CvUnitInfo = None

								for iUnitX in xrange(GC.getNumUnitInfos()):

									CvUnitInfoX = GC.getUnitInfo(iUnitX)
									if CvUnitInfoX.getDomainType() != DomainTypes.DOMAIN_LAND: continue
									if not CvUnitInfoX.getReligionSpreads(eStateReligion): continue

									if CyCity.canTrain(iUnitX, False, False, False, False):

										iValue = CyPlayer.AI_unitValue(iUnitX, UnitAITypes.UNITAI_MISSIONARY, CyArea)

										if iValue > iBestValue:

											iBestValue = iValue
											iBestUnit = iUnitX
											CvUnitInfo = CvUnitInfoX

								if CvUnitInfo:
									popupInfo = CyPopupInfo()
									popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
									popupInfo.setData1(CyCity.getID())
									popupInfo.setData2(OrderTypes.ORDER_TRAIN)
									popupInfo.setData3(iBestUnit)
									popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_MISSIONARY_DEMAND", (GC.getReligionInfo(eStateReligion).getTextKey(), CvUnitInfo.getTextKey(), CyCity.getNameKey())))
									popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
									popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
									popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
									popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
									popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
									popupInfo.addPopup(iPlayer)
									g_iAdvisorNags += 1

			if not CyCity.isProductionBuilding() and CyCity.getOrderQueueLength() <= 1:

				if CyCity.healthRate(False, 0) < 0:

					if (iTurn + 6) % 40 == iTurnFounded % 40:

						iBestValue = 0
						iBestBuilding = -1
						CvBuildingInfo = None

						for iBuildingX in xrange(GC.getNumBuildingInfos()):

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

							if isLimitedWonderClass(CvBuildingInfoX.getBuildingClassType()): continue

							iValue = CvBuildingInfoX.getHealth()
							if iValue <= iBestValue: continue

							if CyCity.canConstruct(iBuildingX, False, False, False):

								iBestValue = iValue
								iBestBuilding = iBuildingX
								CvBuildingInfo = CvBuildingInfoX

						if CvBuildingInfo:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(iBestBuilding)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_UNHEALTHY_CITIZENS_DEMAND", (CyCity.getNameKey(), CvBuildingInfo.getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_UNHEALTHY_DO_SO_NEXT", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_UNHEALTHY_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_UNHEALTHY_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if CyCity.angryPopulation(0) > 0:

					if (iTurn + 9) % 40 == iTurnFounded % 40:

						iBestValue = 0
						iBestBuilding = -1
						CvBuildingInfo = None

						for iBuildingX in xrange(GC.getNumBuildingInfos()):

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

							if isLimitedWonderClass(CvBuildingInfoX.getBuildingClassType()): continue

							iValue = CvBuildingInfoX.getHappiness()
							if iValue <= iBestValue: continue

							if CyCity.canConstruct(iBuildingX, False, False, False):

								iBestValue = iValue
								iBestBuilding = iBuildingX
								CvBuildingInfo = CvBuildingInfoX

						if CvBuildingInfo:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(iBestBuilding)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_UNHAPPY_CITIZENS_DEMAND", (CyCity.getNameKey(), CvBuildingInfo.getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_UNHAPPY_DO_SO_NEXT", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_UNHAPPY_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_UNHEALTHY_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if iTurn < 100 and GC.getTeam(CyPlayer.getTeam()).getHasMetCivCount(True) > 0 and not CyCity.getBuildingDefense():

					if (iTurn + 12) % 40 == iTurnFounded % 40:

						iBestValue = 0
						iBestBuilding = -1
						CvBuildingInfo = None

						for iBuildingX in xrange(GC.getNumBuildingInfos()):

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

							if isLimitedWonderClass(CvBuildingInfoX.getBuildingClassType()): continue

							iValue = CvBuildingInfoX.getDefenseModifier()
							if iValue <= iBestValue: continue

							if CyCity.canConstruct(iBuildingX, False, False, False):

								iBestValue = iValue
								iBestBuilding = iBuildingX
								CvBuildingInfo = CvBuildingInfoX

						if CvBuildingInfo:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(iBestBuilding)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_BUILDING_DEFENSE_DEMAND", (CyCity.getNameKey(), CvBuildingInfo.getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if CyCity.getMaintenance() >= 8:

					if (iTurn + 18) % 40 == iTurnFounded % 40:

						iBestValue = 0
						iBestBuilding = -1
						CvBuildingInfo = None

						for iBuildingX in xrange(GC.getNumBuildingInfos()):

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

							if isLimitedWonderClass(CvBuildingInfoX.getBuildingClassType()): continue

							iValue = CvBuildingInfoX.getMaintenanceModifier()
							if iValue >= iBestValue: continue

							if CyCity.canConstruct(iBuildingX, False, False, False):

								iBestValue = iValue
								iBestBuilding = iBuildingX
								CvBuildingInfo = CvBuildingInfoX

						if CvBuildingInfo:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(iBestBuilding)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_MAINTENANCE_DEMAND", (CyCity.getNameKey(), CvBuildingInfo.getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if CyCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE) == 0 and not CyCity.isOccupation():

					if (iTurn + 21) % 40 == iTurnFounded % 40:

						iBestValue = 0
						iBestBuilding = -1
						CvBuildingInfo = None

						for iBuildingX in xrange(GC.getNumBuildingInfos()):

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

							if isLimitedWonderClass(CvBuildingInfoX.getBuildingClassType()): continue

							iValue = CvBuildingInfoX.getObsoleteSafeCommerceChange(CommerceTypes.COMMERCE_CULTURE)
							if iValue <= iBestValue: continue

							if CyCity.canConstruct(iBuildingX, False, False, False):

								iBestValue = iValue
								iBestBuilding = iBuildingX
								CvBuildingInfo = CvBuildingInfoX

						if CvBuildingInfo:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(iBestBuilding)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_CULTURE_DEMAND", (CyCity.getNameKey(), CvBuildingInfo.getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if CyCity.getBaseCommerceRate(CommerceTypes.COMMERCE_GOLD) > 10:

					if (iTurn + 24) % 40 == iTurnFounded % 40:

						iBestValue = 0
						iBestBuilding = -1
						CvBuildingInfo = None

						for iBuildingX in xrange(GC.getNumBuildingInfos()):

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

							if isLimitedWonderClass(CvBuildingInfoX.getBuildingClassType()): continue

							iValue = CvBuildingInfoX.getCommerceModifier(CommerceTypes.COMMERCE_GOLD)
							if iValue <= iBestValue: continue

							if CyCity.canConstruct(iBuildingX, False, False, False):

								iBestValue = iValue
								iBestBuilding = iBuildingX
								CvBuildingInfo = CvBuildingInfoX

						if CvBuildingInfo:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(iBestBuilding)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_GOLD_DEMAND", (CyCity.getNameKey(), CvBuildingInfo.getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if CyCity.getBaseCommerceRate(CommerceTypes.COMMERCE_RESEARCH) > 10:

					if (iTurn + 30) % 40 == iTurnFounded % 40:

						iBestValue = 0
						iBestBuilding = -1
						CvBuildingInfo = None

						for iBuildingX in xrange(GC.getNumBuildingInfos()):

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

							if isLimitedWonderClass(CvBuildingInfoX.getBuildingClassType()): continue

							iValue = CvBuildingInfoX.getCommerceModifier(CommerceTypes.COMMERCE_RESEARCH)
							if iValue <= iBestValue: continue

							if CyCity.canConstruct(iBuildingX, False, False, False):

								iBestValue = iValue
								iBestBuilding = iBuildingX
								CvBuildingInfo = CvBuildingInfoX

						if CvBuildingInfo:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(iBestBuilding)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_RESEARCH_DEMAND", (CyCity.getNameKey(), CvBuildingInfo.getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if CyCity.countNumWaterPlots() > 10:

					if (iTurn + 33) % 40 == iTurnFounded % 40:

						iBestValue = 0
						iBestBuilding = -1
						CvBuildingInfo = None

						for iBuildingX in xrange(GC.getNumBuildingInfos()):

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

							if isLimitedWonderClass(CvBuildingInfoX.getBuildingClassType()): continue

							iValue = CvBuildingInfoX.getSeaPlotYieldChange(YieldTypes.YIELD_FOOD)
							if iValue <= iBestValue: continue

							if CyCity.canConstruct(iBuildingX, False, False, False):

								iBestValue = iValue
								iBestBuilding = iBuildingX
								CvBuildingInfo = CvBuildingInfoX

						if CvBuildingInfo:
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(CyCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(iBestBuilding)
							popupInfo.setText(TRNSLTR.getText("TXT_KEY_POPUP_WATER_FOOD_DEMAND", (CyCity.getNameKey(), CvBuildingInfo.getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1
