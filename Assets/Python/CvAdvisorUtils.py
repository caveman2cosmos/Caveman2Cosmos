from CvPythonExtensions import *

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

lPopulation = [
	[2000000000, FeatTypes.FEAT_POPULATION_2_BILLION, "TXT_KEY_FEAT_2_BILLION"],
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
	[500000, FeatTypes.FEAT_POPULATION_HALF_MILLION, "TXT_KEY_FEAT_HALF_MILLION"]
]
g_iAdvisorNags = 0

def resetNoLiberateCities():
	global g_listNoLiberateCities
	g_listNoLiberateCities = []

	global lCorporations
	lCorporations = []
	for iI in xrange(GC.getNumBuildingInfos()):
		CvBuildingInfo = GC.getBuildingInfo(iI)
		eCorporation = CvBuildingInfo.getFoundsCorporation()
		if eCorporation > -1 and not GAME.isCorporationFounded(eCorporation):

			bonuses = GC.getCorporationInfo(eCorporation).getPrereqBonuses()
			if not bonuses:
				continue

			for iUnit in xrange(GC.getNumUnitInfos()):
				if GC.getUnitInfo(iUnit).getHasBuilding(iI):
					break
			else: continue

			techs = []
			iTech = CvBuildingInfo.getPrereqAndTech()
			if iTech > -1:
				techs.append(iTech)
			for iTech in CvBuildingInfo.getPrereqAndTechs():
				techs.append(iTech)

			lCorporations.append([eCorporation, techs, iUnit, bonuses])

	global lBonus
	lBonus = []
	lLuxury = []
	lFood = []
	for i in xrange(GC.getNumBonusInfos()):
		if GC.getBonusInfo(i).getHappiness() > 0:
			lLuxury.append(i)
		if GC.getBonusInfo(i).getHealth() > 0:
			lFood.append(i)
	iBonus = GC.getInfoTypeForString("BONUS_COPPER_ORE")
	if iBonus > -1:
		lBonus.append([FeatTypes.FEAT_COPPER_CONNECTED, [iBonus], "TXT_KEY_FEAT_COPPER_CONNECTED"])
	iBonus = GC.getInfoTypeForString("BONUS_HORSE")
	if iBonus > -1:
		lBonus.append([FeatTypes.FEAT_HORSE_CONNECTED, [iBonus], "TXT_KEY_FEAT_HORSE_CONNECTED"])
	iBonus = GC.getInfoTypeForString("BONUS_IRON_ORE")
	if iBonus > -1:
		lBonus.append([FeatTypes.FEAT_IRON_CONNECTED, [iBonus], "TXT_KEY_FEAT_IRON_CONNECTED"])
	if lLuxury:
		lBonus.append([FeatTypes.FEAT_LUXURY_CONNECTED, lLuxury, "TXT_KEY_FEAT_LUXURY_CONNECTED"])
	if lFood:
		lBonus.append([FeatTypes.FEAT_FOOD_CONNECTED, lFood, "TXT_KEY_FEAT_FOOD_CONNECTED"])

	global unitCombatFeats
	unitCombatFeats = []
	unitCombatFeats.append((GC.getInfoTypeForString("UNITCOMBAT_ARCHER"), FeatTypes.FEAT_UNITCOMBAT_ARCHER, "TXT_KEY_FEAT_UNITCOMBAT_ARCHER"))
	unitCombatFeats.append((GC.getInfoTypeForString("UNITCOMBAT_MOUNTED"), FeatTypes.FEAT_UNITCOMBAT_MOUNTED, "TXT_KEY_FEAT_UNITCOMBAT_MOUNTED"))
	unitCombatFeats.append((GC.getInfoTypeForString("UNITCOMBAT_MELEE"), FeatTypes.FEAT_UNITCOMBAT_MELEE, "TXT_KEY_FEAT_UNITCOMBAT_MELEE"))
	unitCombatFeats.append((GC.getInfoTypeForString("UNITCOMBAT_SIEGE"), FeatTypes.FEAT_UNITCOMBAT_SIEGE, "TXT_KEY_FEAT_UNITCOMBAT_SIEGE"))
	unitCombatFeats.append((GC.getInfoTypeForString("UNITCOMBAT_GUN"), FeatTypes.FEAT_UNITCOMBAT_GUN, "TXT_KEY_FEAT_UNITCOMBAT_GUN"))
	unitCombatFeats.append((GC.getInfoTypeForString("UNITCOMBAT_HELICOPTER"), FeatTypes.FEAT_UNITCOMBAT_HELICOPTER, "TXT_KEY_FEAT_UNITCOMBAT_HELICOPTER"))
	unitCombatFeats.append((GC.getInfoTypeForString("UNITCOMBAT_MOTILITY_NAVAL"), FeatTypes.FEAT_UNITCOMBAT_NAVAL, "TXT_KEY_FEAT_UNITCOMBAT_NAVAL"))


def unitBuiltFeats(CyCity, CyUnit):
	iPlayer = CyCity.getOwner()
	CyPlayer = GC.getPlayer(iPlayer)

	for iCombat, eFeat, szTxt in unitCombatFeats:
		if not CyPlayer.isFeatAccomplished(eFeat) and CyUnit.isHasUnitCombat(iCombat):
			CyPlayer.setFeatAccomplished(eFeat, True)
			if not GAME.isNetworkMultiPlayer() and GAME.getElapsedGameTurns() != 0 and iPlayer == GAME.getActivePlayer() and CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(eFeat)
				popupInfo.setData2(CyCity.getID())
				popupInfo.setText(TRNSLTR.getText(szTxt, (CyUnit.getNameKey(), CyCity.getNameKey(),)))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(iPlayer)

	if not CyPlayer.isFeatAccomplished(FeatTypes.FEAT_UNIT_PRIVATEER):
		if GC.getUnitInfo(CyUnit.getUnitType()).isHiddenNationality() and CyUnit.getDomainType() == DomainTypes.DOMAIN_SEA:
			CyPlayer.setFeatAccomplished(FeatTypes.FEAT_UNIT_PRIVATEER, True)
			if not GAME.isNetworkMultiPlayer() and GAME.getElapsedGameTurns() != 0 and iPlayer == GAME.getActivePlayer() and CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(FeatTypes.FEAT_UNIT_PRIVATEER)
				popupInfo.setData2(CyCity.getID())
				popupInfo.setText(TRNSLTR.getText("TXT_KEY_FEAT_UNIT_PRIVATEER", (CyUnit.getNameKey(), CyCity.getNameKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(iPlayer)

	if not CyPlayer.isFeatAccomplished(FeatTypes.FEAT_UNIT_SPY):
		if GC.getUnitInfo(CyUnit.getUnitType()).isSpy():
			CyPlayer.setFeatAccomplished(FeatTypes.FEAT_UNIT_SPY, True)
			if not GAME.isNetworkMultiPlayer() and GAME.getElapsedGameTurns() != 0 and iPlayer == GAME.getActivePlayer() and CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(FeatTypes.FEAT_UNIT_SPY)
				popupInfo.setData2(CyCity.getID())
				popupInfo.setText(TRNSLTR.getText("TXT_KEY_FEAT_UNIT_SPY", (CyUnit.getNameKey(), CyCity.getNameKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(iPlayer)


def endTurnFeats(iPlayer):
	global g_iAdvisorNags
	g_iAdvisorNags = 0

	CyPlayer = GC.getPlayer(iPlayer)
	CyCity0 = CyPlayer.getCapitalCity()
	if CyCity0 is None: return

	# Population feat
	lRealPopulation = CyPlayer.getRealPopulation()
	for item in lPopulation:
		if CyPlayer.isFeatAccomplished(item[1]): break
		if lRealPopulation > item[0]:
			CyPlayer.setFeatAccomplished(item[1], True)
			if not GAME.isNetworkMultiPlayer() and iPlayer == GAME.getActivePlayer() and CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(item[1])
				popupInfo.setText(TRNSLTR.getText(item[2], (CyPlayer.getCivilizationDescriptionKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(iPlayer)
	# Trade Route
	if not CyPlayer.isFeatAccomplished(FeatTypes.FEAT_TRADE_ROUTE):
		for CyCityX in CyPlayer.cities():
			if not CyCityX.isCapital():
				if CyCityX.isConnectedToCapital(iPlayer):
					CyPlayer.setFeatAccomplished(FeatTypes.FEAT_TRADE_ROUTE, True)
					if not GAME.isNetworkMultiPlayer() and iPlayer == GAME.getActivePlayer() and CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
						popupInfo = CyPopupInfo()
						popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
						popupInfo.setData1(FeatTypes.FEAT_TRADE_ROUTE)
						popupInfo.setData2(CyCityX.getID())
						popupInfo.setText(TRNSLTR.getText("TXT_KEY_FEAT_TRADE_ROUTE", (CyCityX.getNameKey(), )))
						popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
						popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
						popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
						popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
						popupInfo.addPopup(iPlayer)
					break
	# First Bonuses Obtained
	for item in lBonus:
		if CyPlayer.isFeatAccomplished(item[0]): continue
		for iBonus in item[1]:
			if CyCity0.hasBonus(iBonus):
				CyPlayer.setFeatAccomplished(item[0], True)
				if not GAME.isNetworkMultiPlayer() and iPlayer == GAME.getActivePlayer() and CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
					popupInfo.setData1(item[0])
					popupInfo.setData2(CyCity0.getID())
					popupInfo.setText(TRNSLTR.getText(item[2], (GC.getBonusInfo(iBonus).getTextKey(),)))
					popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
					popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
					popupInfo.addPopup(iPlayer)
					break
	# Corporations
	if not CyPlayer.isFeatAccomplished(FeatTypes.FEAT_CORPORATION_ENABLED):
		global lCorporations
		eTeam = CyPlayer.getTeam()
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
					CyPlayer.setFeatAccomplished(FeatTypes.FEAT_CORPORATION_ENABLED, True)
					szBonusList = u""
					for j in xrange(len(item[3])):
						eBonus = item[3][j]
						szBonusList += GC.getBonusInfo(eBonus).getDescription()
						if j != len(item[3]) - 1:
							szBonusList += TRNSLTR.getText("TXT_KEY_OR", ())

					szFounder = GC.getUnitInfo(item[2]).getTextKey()

					if not GAME.isNetworkMultiPlayer() and iPlayer == GAME.getActivePlayer() and CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
						popupInfo = CyPopupInfo()
						popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
						popupInfo.setData1(FeatTypes.FEAT_CORPORATION_ENABLED)
						popupInfo.setData2(CyCity0.getID())
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

	if g_iAdvisorNags > 1 or CyCity.isDisorder():
		return
	CyPlayer = GC.getPlayer(iPlayer)

	if CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):

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

							for iUnitX in xrange(GC.getNumUnitInfos()):

								if isLimitedUnit(iUnitX): continue

								if GC.getUnitInfo(iUnitX).getDomainType() == DomainTypes.DOMAIN_LAND:

									if CyCity.canTrain(iUnitX, False, False, False, False):

										if CyCity.getFirstUnitOrder(iUnitX) == -1:

											iValue = CyPlayer.AI_unitValue(iUnitX, UnitAITypes.UNITAI_SETTLE, CyArea)

											if iValue > iBestValue:

												iBestValue = iValue
												eBestUnit = iUnitX

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
						eBestUnit = -1

						if CyCity.AI_countBestBuilds(CyArea) > 3:
							iBestValue = 0
							for iUnit in xrange(GC.getNumUnitInfos()):
								if isLimitedUnit(iUnit) or GC.getUnitInfo(iUnit).getDomainType() != DomainTypes.DOMAIN_LAND:
									continue

								if CyCity.getFirstUnitOrder(iUnit) == -1 and CyCity.canTrain(iUnit, False, False, False, False):
									iValue = CyPlayer.AI_unitValue(iUnit, UnitAITypes.UNITAI_WORKER, CyArea)
									if iValue > iBestValue:
										iBestValue = iValue
										eBestUnit = iUnit

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

						for iUnit in xrange(GC.getNumUnitInfos()):

							if isLimitedUnit(iUnit) or GC.getUnitInfo(iUnit).getDomainType() != DomainTypes.DOMAIN_LAND:
								continue
							if CyCity.canTrain(iUnit, False, False, False, False):
								iValue = CyPlayer.AI_unitValue(iUnit, UnitAITypes.UNITAI_CITY_DEFENSE, CyArea) * 2
								iValue += CyPlayer.AI_unitValue(iUnit, UnitAITypes.UNITAI_ATTACK, CyArea)
								if iValue > iBestValue:
									iBestValue = iValue
									eBestUnit = iUnit

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

							if isLimitedWonder(iBuildingX): continue

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

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

							if isLimitedWonder(iBuildingX): continue

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

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

							if isLimitedWonder(iBuildingX): continue

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

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

							if isLimitedWonder(iBuildingX): continue

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

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

				if CyCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE) < 10 and not CyCity.isOccupation():

					if (iTurn + 21) % 40 == iTurnFounded % 40:

						iBestValue = 0
						iBestBuilding = -1
						CvBuildingInfo = None

						for iBuildingX in xrange(GC.getNumBuildingInfos()):

							if isLimitedWonder(iBuildingX): continue

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

							iValue = CvBuildingInfoX.getCommerceChange(CommerceTypes.COMMERCE_CULTURE)
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

							if isLimitedWonder(iBuildingX): continue

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

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

							if isLimitedWonder(iBuildingX): continue

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

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

							if isLimitedWonder(iBuildingX): continue

							CvBuildingInfoX = GC.getBuildingInfo(iBuildingX)

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
