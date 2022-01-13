## Civ4lerts
## This class extends the built in event manager and overrides various
## event handlers to display alerts about important game situations.
##
## [*] = Already implemented in the Civ4lerts mod
## [o] = Partially implemented in the Civ4lerts mod
## [x] = Already implemented in CivIV
## [?] = Not sure if this applies in CivIV
##
## Golden Age turns left
## At Year 1000 B.C. (QSC Save Submission)
## Within 10 tiles of domination limit
## There is new technology for sale
## There is a new luxury resource for sale
## There is a new strategic resource for sale
## There is a new bonus resource for sale
## We can sell a technology
## We can sell a luxury resource
## We can sell a strategic resource
## We can sell a bonus resource
## [*] Rival has lots of cash
## [*] Rival has lots of cash per turn
## [x] Rival has changed civics
## Rival has entered a new Era
## Trade deal expires next turn
## [o] Enemy at war is willing to negotiate
## [x] There are foreign units in our territory
## City is about to riot or rioting
## [*] City has grown or shrunk
## City has shrunk
## [*] City is unhealthy
## [*] City is angry
## City specialists reassigned
## [*] City is about to grow
## City is about to starve
## [*] City is about to grow into unhealthiness
## [*] City is about to grow into anger
## City is in resistance
## [?] City is wasting food
## City is working unimproved tiles
## Disconnected resources in our territory
## City is about to produce a great person
##
## Other:
## City is under cultural pressure


from CvPythonExtensions import *
import CvUtil
import AttitudeUtil
import BugCore
import BugUtil
import CityUtil
import TradeUtil

# Must set alerts to "not immediate" to have icons show up
# Need a healthy person icon
HEALTHY_ICON = "Art/Interface/Buttons/General/unhealthy_person.dds"
UNHEALTHY_ICON = "Art/Interface/Buttons/General/unhealthy_person.dds"

HAPPY_ICON = "Art/Interface/Buttons/General/happy_person.dds"
UNHAPPY_ICON = "Art/Interface/mainscreen/cityscreen/angry_citizen.dds"

### Globals

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

EVENT_MESSAGE_TIME_LONG = GC.getDefineINT("EVENT_MESSAGE_TIME_LONG")
Civ4lertsOpt = BugCore.game.Civ4lerts


## Initialization

class Civ4lerts:

	def __init__(self, eventManager):
		cityEvent = BeginActivePlayerTurnCityAlertManager(eventManager)
		cityEvent.add(CityOccupation(eventManager))
		cityEvent.add(CityGrowth(eventManager))
		cityEvent.add(CityHealthiness(eventManager))
		cityEvent.add(CityHappiness(eventManager))
		cityEvent.add(CanHurryPopulation(eventManager))
		cityEvent.add(CanHurryGold(eventManager))

		cityEvent = EndTurnReadyCityAlertManager(eventManager)
		cityEvent.add(CityPendingGrowth(eventManager))

		GoldTrade(eventManager)
		GoldPerTurnTrade(eventManager)
		RefusesToTalk(eventManager)
		WorstEnemy(eventManager)


## Displaying Alert Messages

def addMessageAtPlot(iPlayer, message, icon, iX, iY):
	"Displays an on-screen message with a popup icon that zooms to the given plot."
	addMessage(iPlayer, message, icon, iX, iY, True, True)

def addMessage(iPlayer, szTxt, icon=None, iX=-1, iY=-1, bOffArrow=False, bOnArrow=False):
	"Displays an on-screen message."
	"""
	Make these alerts optionally show a delayable popup with various options.
	a) show:

	Happy: Zoom to City, Turn OFF avoid growth, Whip (maybe?), Ignore
	Unhappy:  Zoom to City, Turn on Avoid Growth, Suggest cheapest military unit (with right civic), Open Resources screen in FA, Ignore. (for future = suggest building)

	Healthy: Zoom to City, Turn OFF avoid growth, Ignore
	Unhealthy:  Zoom to City, Turn on Avoid Growth, Whip population, Open Resources screen in FA, Ignore. (for future = suggest building)

	Growth: Zoom to City, Turn on avoid Growth, Whip, Ignore
	Starvation: Zoom to City, Turn on avoid Growth, Ignore

	Culture:  Zoom to City, Ignore
	"""
	CvUtil.sendMessage(szTxt, iPlayer, EVENT_MESSAGE_TIME_LONG, icon, -1, iX, iY, bOffArrow, bOnArrow)

## Base Alert Class
class AbstractStatefulAlert:
	"""
	Provides a base class and several convenience functions for
	implementing an alert that retains state between turns.
	"""
	def __init__(self, eventManager):
		eventManager.addEventHandler("GameStart", self.onGameStart)
		eventManager.addEventHandler("OnLoad", self.onLoadGame)

	def onGameStart(self, argsList):
		self._init()
		self._reset()

	def onLoadGame(self, argsList):
		self._init()
		self._reset()
		return 0

	def _init(self):
		"Initializes globals that could not be done in __init__."
		pass

	def _reset(self):
		"Resets the state for this alert."
		pass


## City Alert Managers

def getCityId(city):
	return (city.getOwner(), city.getID())

class AbstractCityAlertManager(AbstractStatefulAlert):
	"""
	Triggered when cities are acquired or lost, this event manager passes
	each off to a set of alert checkers.

	All of the alerts are reset when the game is loaded or started.
	"""
	def __init__(self, eventManager):
		AbstractStatefulAlert.__init__(self, eventManager)
		eventManager.addEventHandler("cityAcquiredAndKept", self.onCityAcquiredAndKept)
		eventManager.addEventHandler("cityLost", self.onCityLost)
		self.alerts = []

	def add(self, alert):
		self.alerts.append(alert)
		alert.init()

	def onCityAcquiredAndKept(self, argsList):
		iPlayer, city = argsList
		if iPlayer == GAME.getActivePlayer():
			self._resetCity(city)

	def onCityLost(self, argsList):
		city = argsList[0]
		iPlayer = GAME.getActivePlayer()
		if iPlayer == city.getOwner():
			self._discardCity(city)

	def checkAllActivePlayerCities(self):
		"Loops over active player's cities, telling each alert to perform its check."
		ePlayer = GAME.getActivePlayer()
		player = GC.getActivePlayer()
		for city in player.cities():
			for alert in self.alerts:
				alert.checkCity(city.getID(), city, ePlayer, player)

	def _init(self):
		"Initializes each alert."
		for alert in self.alerts:
			alert.init()

	def _reset(self):
		"Resets each alert."
		for alert in self.alerts:
			alert.reset()

	def _resetCity(self, city):
		"tells each alert to check the state of the given city -- no alerts are displayed."
		for alert in self.alerts:
			alert.resetCity(city)

	def _discardCity(self, city):
		"tells each alert to discard the state of the given city."
		for alert in self.alerts:
			alert.discardCity(city)

class BeginActivePlayerTurnCityAlertManager(AbstractCityAlertManager):
	"""
	Extends AbstractCityAlertManager to loop over all of the active player's
	cities at the start of their turn.
	"""
	def __init__(self, eventManager):
		AbstractCityAlertManager.__init__(self, eventManager)
		eventManager.addEventHandler("BeginActivePlayerTurn", self.onBeginActivePlayerTurn)

	def onBeginActivePlayerTurn(self, argsList):
		"Loops over active player's cities, telling each to perform its check."
		self.checkAllActivePlayerCities()

class EndTurnReadyCityAlertManager(AbstractCityAlertManager):
	"""
	Extends AbstractCityAlertManager to loop over all of the active player's
	cities at the end of their turn (the moment the End Turn button turns red).
	"""
	def __init__(self, eventManager):
		AbstractCityAlertManager.__init__(self, eventManager)
		eventManager.addEventHandler("endTurnReady", self.onEndTurnReady)

	def onEndTurnReady(self, argsList):
		"Loops over active player's cities, telling each to perform its check."
		self.checkAllActivePlayerCities()


## City Alerts

class AbstractCityAlert:
	"""
	Tracks cities from turn-to-turn and checks each at the end of every game turn
	to see if the alert should be displayed.
	"""
	def __init__(self, eventManager):
		"Performs static initialization that doesn't require game data."
		pass

	def checkCity(self, cityId, city, iPlayer, player):
		"Checks the city, updates its tracked state and possibly displays an alert."
		pass

	def init(self):
		"Initializes globals that could not be done in __init__ and resets the data."
		self._beforeReset()

	def reset(self):
		"Clears state kept for each city."
		self._beforeReset()
		for city in GC.getActivePlayer().cities():
			self.resetCity(city)

	def _beforeReset(self):
		"Performs clearing of state before looping over cities."
		pass

	def resetCity(self, city):
		"Checks the city and updates its tracked state."
		pass

	def discardCity(self, city):
		"Discards the tracked state of the city."
		pass

class AbstractCityTestAlert(AbstractCityAlert):
	"""
	Extends the basic city alert by applying a boolean test to each city, tracking the results,
	and displaying an alert whenever a city switches or will switch state on the following turn.

	State: set of city IDs that pass the test.
	"""
	def __init__(self, eventManager):
		AbstractCityAlert.__init__(self, eventManager)

	def checkCity(self, cityId, city, iPlayer, player):
		message = None
		passes = self._passesTest(city)
		passed = cityId in self.cities
		if passes != passed:
			# City switched this turn, save new state and display an alert
			if passes:
				self.cities.add(cityId)
				if self._isShowAlert(passes):
					message, icon = self._getAlertMessageIcon(city, passes)
			else:
				self.cities.discard(cityId)
				if self._isShowAlert(passes):
					message, icon = self._getAlertMessageIcon(city, passes)
		elif self._isShowPendingAlert(passes):
			# See if city will switch next turn
			willPass = self._willPassTest(city)
			if passed != willPass:
				message, icon = self._getPendingAlertMessageIcon(city, willPass)
		if message:
			addMessageAtPlot(iPlayer, message, icon, city.getX(), city.getY())

	def _passedTest(self, cityId):
		"Returns True if the city passed the test last turn."
		return cityId in self.cities

	def _passesTest(self, city):
		"Returns True if the city passes the test."
		return False

	def _willPassTest(self, city):
		"Returns True if the city will pass the test next turn based on current conditions."
		return False

	def _beforeReset(self):
		self.cities = set()

	def resetCity(self, city):
		if self._passesTest(city):
			self.cities.add(getCityId(city))

	def discardCity(self, city):
		self.cities.discard(getCityId(city))

	def _isShowAlert(self, passes):
		"Returns True if the alert is enabled."
		return False

	def _getAlertMessageIcon(self, city, passes):
		"Returns a tuple of the message and icon to use for the alert."
		return (None, None)

	def _isShowPendingAlert(self, passes):
		"Returns True if the alert is enabled."
		return False

	def _getPendingAlertMessageIcon(self, city, passes):
		"Returns a tuple of the message and icon to use for the pending alert."
		return (None, None)

# Population

class CityPendingGrowth(AbstractCityAlert):
	"""
	Displays an alert when a city's population will change next turn.
	State: None.
	"""
	def __init__(self, eventManager):
		AbstractCityAlert.__init__(self, eventManager)

	def checkCity(self, cityId, city, iPlayer, player):
		if Civ4lertsOpt.isShowCityPendingGrowthAlert():
			if CityUtil.willGrowThisTurn(city):

				message = TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_PENDING_GROWTH", (city.getName(), city.getPopulation() + 1))
				addMessageAtPlot(iPlayer, message, "Art/Interface/Symbols/Food/food05.dds", city.getX(), city.getY())

			elif CityUtil.willShrinkThisTurn(city):

				message = TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_PENDING_SHRINKAGE", (city.getName(), city.getPopulation() - 1))
				addMessageAtPlot(iPlayer, message, "Art/Interface/Symbols/Food/food05.dds", city.getX(), city.getY())

class CityGrowth(AbstractCityAlert):
	"""
	Displays an alert when a city's population changes.
	State: map of populations by city ID.
	"""
	def __init__(self, eventManager):
		AbstractCityAlert.__init__(self, eventManager)

	def checkCity(self, cityId, city, iPlayer, player):
		if cityId not in self.populations:
			self.resetCity(city)
		else:
			iPop = city.getPopulation()
			iOldPop = self.populations[cityId]
			iWhipCounter = city.getHurryAngerTimer()
			iConscriptCounter = city.getConscriptAngerTimer()

			bWhipOrDraft = False
			if iWhipCounter > self.CityWhipCounter[cityId] or iConscriptCounter > self.CityConscriptCounter[cityId]:
				bWhipOrDraft = True

			if Civ4lertsOpt.isShowCityGrowthAlert():
				if iPop > iOldPop:
					message = TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_GROWTH", (city.getName(), iPop))
					addMessageAtPlot(iPlayer, message, "Art/Interface/Symbols/Food/food05.dds", city.getX(), city.getY())

				elif iPop < iOldPop and not bWhipOrDraft:
					message = TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_SHRINKAGE", (city.getName(), iPop))
					addMessageAtPlot(iPlayer, message, "Art/Interface/Symbols/Food/food05.dds", city.getX(), city.getY())

			self.populations[cityId] = iPop
			self.CityWhipCounter[cityId] = iWhipCounter
			self.CityConscriptCounter[cityId] = iConscriptCounter

	def _beforeReset(self):
		self.populations = dict()
		self.CityWhipCounter = dict()
		self.CityConscriptCounter = dict()

	def resetCity(self, city):
		cityId = getCityId(city)
		self.populations[cityId] = city.getPopulation()
		self.CityWhipCounter[cityId] = city.getHurryAngerTimer()
		self.CityConscriptCounter[cityId] = city.getConscriptAngerTimer()

	def discardCity(self, city):
		cityId = getCityId(city)
		if cityId in self.populations:
			del self.populations[cityId], self.CityWhipCounter[cityId], self.CityConscriptCounter[cityId]

# Happiness and Healthiness

class CityHappiness(AbstractCityTestAlert):
	"""
	Displays an event when a city goes from happy to angry or vice versa.

	Test: True if the city is unhappy.
	"""
	def __init__(self, eventManager):
		AbstractCityTestAlert.__init__(self, eventManager)

	def init(self):
		AbstractCityAlert.init(self)
		self.kiTempHappy = GC.getDefineINT("TEMP_HAPPY")

	def _passesTest(self, city):
		return city.angryPopulation(0) > 0

	def _willPassTest(self, city):
		if CityUtil.willGrowThisTurn(city):
			iExtra = 1
		elif CityUtil.willShrinkThisTurn(city):
			iExtra = -1
		else:
			iExtra = 0
		iHappy = city.happyLevel()
		iUnhappy = city.unhappyLevel(iExtra)
		iTimer = city.getHurryAngerTimer()
		if iUnhappy > 0 and iTimer > 0 and not iTimer % city.flatHurryAngerLength():
			iUnhappy -= 1
		iTimer = city.getConscriptAngerTimer()
		if iUnhappy > 0 and iTimer > 0 and not iTimer % city.flatConscriptAngerLength():
			iUnhappy -= 1
		iTimer = city.getDefyResolutionAngerTimer()
		if iUnhappy > 0 and iTimer > 0 and not iTimer % city.flatDefyResolutionAngerLength():
			iUnhappy -= 1
		if iUnhappy > 0 and city.getEspionageHappinessCounter() > 0:
			iUnhappy -= 1
		if iHappy > 0 and city.getHappinessTimer() == 1:
			iHappy -= self.kiTempHappy
		if iHappy < 0:
			iHappy = 0
		if iUnhappy < 0:
			iUnhappy = 0
		return iHappy < iUnhappy

	def _isShowAlert(self, passes):
		return Civ4lertsOpt.isShowCityHappinessAlert()

	def _getAlertMessageIcon(self, city, passes):
		if passes:
			return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_UNHAPPY", (city.getName(), )), UNHAPPY_ICON)
		return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_HAPPY", (city.getName(), )), HAPPY_ICON)

	def _isShowPendingAlert(self, passes):
		return Civ4lertsOpt.isShowCityPendingHappinessAlert()

	def _getPendingAlertMessageIcon(self, city, passes):
		if passes:
			return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_PENDING_UNHAPPY", (city.getName(), )), UNHAPPY_ICON)
		return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_PENDING_HAPPY", (city.getName(), )), HAPPY_ICON)

class CityHealthiness(AbstractCityTestAlert):
	"""
	Displays an event when a city goes from healthy to sick or vice versa.

	Test: True if the city is unhealthy.
	"""
	def __init__(self, eventManager):
		AbstractCityTestAlert.__init__(self, eventManager)

	def _passesTest(self, city):
		return city.healthRate(False, 0) < 0

	def _willPassTest(self, city):
		if CityUtil.willGrowThisTurn(city):
			iExtra = 1
		elif CityUtil.willShrinkThisTurn(city):
			iExtra = -1
		else:
			iExtra = 0
		# badHealth() doesn't take iExtra!
		iHealthRate = city.healthRate(False, iExtra)
		if city.getEspionageHealthCounter() > 0:
			iHealthRate += 1
		return iHealthRate < 0

	def _isShowAlert(self, passes):
		return Civ4lertsOpt.isShowCityHealthinessAlert()

	def _getAlertMessageIcon(self, city, passes):
		if passes:
			return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_UNHEALTHY", (city.getName(), )), UNHEALTHY_ICON)
		return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_HEALTHY", (city.getName(), )), HEALTHY_ICON)

	def _isShowPendingAlert(self, passes):
		return Civ4lertsOpt.isShowCityPendingHealthinessAlert()

	def _getPendingAlertMessageIcon(self, city, passes):
		if passes:
			return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_PENDING_UNHEALTHY", (city.getName(), )), UNHEALTHY_ICON)
		return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_PENDING_HEALTHY", (city.getName(), )), HEALTHY_ICON)

# Occupation

class CityOccupation(AbstractCityTestAlert):
	"""
	Displays an alert when a city switches to/from occupation.

	Test: True if the city is under occupation.
	"""
	def __init__(self, eventManager):
		AbstractCityTestAlert.__init__(self, eventManager)

	def _passesTest(self, city):
		return city.isOccupation()

	def _willPassTest(self, city):
		return city.isOccupation() and city.getOccupationTimer() > 1

	def _isShowAlert(self, passes):
		return Civ4lertsOpt.isShowCityOccupationAlert()

	def _getAlertMessageIcon(self, city, passes):
		if passes:
			print "%s passed occupation test, ignoring" % city.getName()
			return (None, None)
		return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_PACIFIED", (city.getName(), )), HAPPY_ICON)

	def _isShowPendingAlert(self, passes):
		return Civ4lertsOpt.isShowCityPendingOccupationAlert()

	def _getPendingAlertMessageIcon(self, city, passes):
		if passes:
			print "[WARN] %s passed pending occupation test, ignoring" % city.getName()
			return (None, None)
		return (TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_PENDING_PACIFIED", (city.getName(), )), HAPPY_ICON)

# Hurrying Production

class AbstractCanHurry(AbstractCityTestAlert):
	"""
	Displays an alert when a city can hurry the current production item.

	Test: True if the city can hurry.
	"""
	def __init__(self, eventManager):
		AbstractCityTestAlert.__init__(self, eventManager)
		eventManager.addEventHandler("cityBuildingUnit", self.onCityBuildingUnit)
		eventManager.addEventHandler("cityBuildingBuilding", self.onCityBuildingBuilding)
		eventManager.addEventHandler("cityBuildingProject", self.onCityBuildingProject)
		eventManager.addEventHandler("cityBuildingProcess", self.onCityBuildingProcess)

	def init(self, szHurryType):
		AbstractCityAlert.init(self)
		self.keHurryType = GC.getInfoTypeForString(szHurryType)

	def onCityBuildingUnit(self, argsList):
		city = argsList[0]
		#iUnit = argsList[1]
		self._onItemStarted(city)

	def onCityBuildingBuilding(self, argsList):
		city = argsList[0]
		#iBuilding = argsList[1]
		self._onItemStarted(city)

	def onCityBuildingProject(self, argsList):
		city = argsList[0]
		#iProject = argsList[1]
		self._onItemStarted(city)

	def onCityBuildingProcess(self, argsList):
		city = argsList[0]
		#iProcess = argsList[1]
		self._onItemStarted(city)

	def _onItemStarted(self, city):
		if city.getOwner() == GAME.getActivePlayer():
			self.discardCity(city)

	def _passesTest(self, city):
		return city.canHurry(self.keHurryType, False)

	def _getAlertMessageIcon(self, city, passes):
		if passes:
			info = None
			if city.isProductionBuilding():
				iType = city.getProductionBuilding()
				if iType >= 0:
					info = GC.getBuildingInfo(iType)
			elif city.isProductionUnit():
				iType = city.getProductionUnit()
				if iType >= 0:
					info = GC.getUnitInfo(iType)
			elif city.isProductionProject():
				# Can't hurry projects, but just in case
				iType = city.getProductionProject()
				if iType >= 0:
					info = GC.getProjectInfo(iType)
			if info:
				return (self._getAlertMessage(city, info), info.getButton())
		return (None, None)

class CanHurryPopulation(AbstractCanHurry):
	"""
	Displays an alert when a city can hurry using population.
	"""
	def __init__(self, eventManager):
		AbstractCanHurry.__init__(self, eventManager)

	def init(self):
		AbstractCanHurry.init(self, "HURRY_POPULATION")

	def _isShowAlert(self, passes):
		return passes and Civ4lertsOpt.isShowCityCanHurryPopAlert()

	def _getAlertMessage(self, city, info):
		iPop = city.hurryPopulation(self.keHurryType)
		iOverflow = city.hurryProduction(self.keHurryType) - city.productionLeft()
		if Civ4lertsOpt.isWhipAssistOverflowCountCurrentProduction():
			iOverflow = iOverflow + city.getCurrentProductionDifference(True, False)
		iAnger = city.getHurryAngerTimer() + city.flatHurryAngerLength()
		iMaxOverflow = city.getMaxProductionOverflow()
		iOverflowGold = max(0, iOverflow - iMaxOverflow) * GC.getDefineINT("MAXED_UNIT_GOLD_PERCENT") / 100
		iOverflow = 100 * iMaxOverflow / city.getBaseYieldRateModifier(YieldTypes.YIELD_PRODUCTION, 0)
		if iOverflowGold > 0:
			return TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_CAN_HURRY_POP_PLUS_GOLD", (city.getName(), info.getDescription(), iPop, iOverflow, iAnger, iOverflowGold))

		return TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_CAN_HURRY_POP", (city.getName(), info.getDescription(), iPop, iOverflow, iAnger))

class CanHurryGold(AbstractCanHurry):
	"""
	Displays an alert when a city can hurry using gold.
	"""
	def __init__(self, eventManager):
		AbstractCanHurry.__init__(self, eventManager)

	def init(self):
		AbstractCanHurry.init(self, "HURRY_GOLD")

	def _isShowAlert(self, passes):
		return passes and Civ4lertsOpt.isShowCityCanHurryGoldAlert()

	def _getAlertMessage(self, city, info):
		iGold = city.getHurryGold(self.keHurryType)
		return TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_CITY_CAN_HURRY_GOLD", (city.getName(), info.getDescription(), iGold))


## Trading Gold

class GoldTrade(AbstractStatefulAlert):
	"""
	Displays an alert when a civilization has a significant increase
	in gold available for trade since the last alert.
	"""
	def __init__(self, eventManager):
		AbstractStatefulAlert.__init__(self, eventManager)
		eventManager.addEventHandler("BeginActivePlayerTurn", self.onBeginActivePlayerTurn)

	def onBeginActivePlayerTurn(self, argsList):
		if (not Civ4lertsOpt.isShowGoldTradeAlert()):
			return
		playerID = GAME.getActivePlayer()
		for rival in TradeUtil.getGoldTradePartners(playerID):
			rivalID = rival.getID()
			oldMaxGoldTrade = self._getMaxGoldTrade(playerID, rivalID)
			newMaxGoldTrade = rival.AI_maxGoldTrade(playerID)
			deltaMaxGoldTrade = newMaxGoldTrade - oldMaxGoldTrade
			if deltaMaxGoldTrade >= Civ4lertsOpt.getGoldTradeThreshold():
				message = TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_GOLD_TRADE", (rival.getName(), newMaxGoldTrade))
				addMessage(playerID, message)
				self._setMaxGoldTrade(playerID, rivalID, newMaxGoldTrade)
			elif newMaxGoldTrade < oldMaxGoldTrade:
				self._setMaxGoldTrade(playerID, rivalID, newMaxGoldTrade)

	def _reset(self):
		self.maxGoldTrade = {}
		for player in range(GC.getMAX_PLAYERS()):
			self.maxGoldTrade[player] = {}
			for rival in range(GC.getMAX_PLAYERS()):
				self._setMaxGoldTrade(player, rival, 0)

	def _getMaxGoldTrade(self, player, rival):
		return self.maxGoldTrade[player][rival]

	def _setMaxGoldTrade(self, player, rival, value):
		self.maxGoldTrade[player][rival] = value

class GoldPerTurnTrade(AbstractStatefulAlert):
	"""
	Displays an alert when a civilization has a significant increase
	in gold per turn available for trade since the last alert.
	"""
	def __init__(self, eventManager):
		AbstractStatefulAlert.__init__(self, eventManager)
		eventManager.addEventHandler("BeginActivePlayerTurn", self.onBeginActivePlayerTurn)

	def onBeginActivePlayerTurn(self, argsList):
		if (not Civ4lertsOpt.isShowGoldPerTurnTradeAlert()):
			return
		playerID = GAME.getActivePlayer()
		for rival in TradeUtil.getGoldTradePartners(playerID):
			rivalID = rival.getID()
			oldMaxGoldPerTurnTrade = self._getMaxGoldPerTurnTrade(playerID, rivalID)
			newMaxGoldPerTurnTrade = rival.AI_maxGoldPerTurnTrade(playerID)
			deltaMaxGoldPerTurnTrade = newMaxGoldPerTurnTrade - oldMaxGoldPerTurnTrade
			if (deltaMaxGoldPerTurnTrade >= Civ4lertsOpt.getGoldPerTurnTradeThreshold()):
				message = TRNSLTR.getText("TXT_KEY_CIV4LERTS_ON_GOLD_PER_TURN_TRADE", (rival.getName(), newMaxGoldPerTurnTrade))
				addMessage(playerID, message)
				self._setMaxGoldPerTurnTrade(playerID, rivalID, newMaxGoldPerTurnTrade)
			else:
				maxGoldPerTurnTrade = min(oldMaxGoldPerTurnTrade, newMaxGoldPerTurnTrade)
				self._setMaxGoldPerTurnTrade(playerID, rivalID, maxGoldPerTurnTrade)

	def _reset(self):
		self.maxGoldPerTurnTrade = {}
		for player in range(GC.getMAX_PC_PLAYERS()):
			self.maxGoldPerTurnTrade[player] = {}
			for rival in range(GC.getMAX_PC_PLAYERS()):
				self._setMaxGoldPerTurnTrade(player, rival, 0)

	def _getMaxGoldPerTurnTrade(self, player, rival):
		return self.maxGoldPerTurnTrade[player][rival]

	def _setMaxGoldPerTurnTrade(self, player, rival, value):
		self.maxGoldPerTurnTrade[player][rival] = value


## Diplomacy

class RefusesToTalk(AbstractStatefulAlert):
	"""
	Displays an alert when a civilization cuts off or reestablishes communication.
	"""
	def __init__(self, eventManager):
		AbstractStatefulAlert.__init__(self, eventManager)
		#RevolutionDCM start - start as minors fix
		self._reset()
		#RevolutionDCM end - start as minors fix
		eventManager.addEventHandler("BeginActivePlayerTurn", self.onBeginActivePlayerTurn)
		eventManager.addEventHandler("changeWar", self.onChangeWar)
		eventManager.addEventHandler("cityRazed", self.onCityRazed)
		eventManager.addEventHandler("DealCanceled", self.onDealCanceled)
		eventManager.addEventHandler("EmbargoAccepted", self.onEmbargoAccepted)

	def onBeginActivePlayerTurn(self, argsList):
		self.check()

	def onChangeWar(self, argsList):
		bIsWar, eTeam, eRivalTeam = argsList
		self.checkIfIsAnyOrHasMetAllTeams(eTeam, eRivalTeam)

	def onCityRazed(self, argsList):
		city, iPlayer = argsList
		self.checkIfIsAnyOrHasMetAllTeams(city.getTeam(), GC.getPlayer(iPlayer).getTeam())

	def onDealCanceled(self, argsList):
		eOfferPlayer, eTargetPlayer, pTrade = argsList
		if eOfferPlayer != -1 and eTargetPlayer != -1:
			self.checkIfIsAnyOrHasMetAllTeams(GC.getPlayer(eOfferPlayer).getTeam(), GC.getPlayer(eTargetPlayer).getTeam())

	def onEmbargoAccepted(self, argsList):
		eOfferPlayer, eTargetPlayer, pTrade = argsList
		self.checkIfIsAnyOrHasMetAllTeams(GC.getPlayer(eOfferPlayer).getTeam(), GC.getPlayer(eTargetPlayer).getTeam())

	def checkIfIsAnyOrHasMetAllTeams(self, *eTeams):
		"""
		Calls check() only if the active team is any or has met all of the given teams.
		"""
		iActiveTeam = GC.getGame().getActiveTeam()
		activeTeam = GC.getTeam(iActiveTeam)
		for eTeam in eTeams:
			if iActiveTeam != eTeam and eTeam >= 0 and not activeTeam.isHasMet(eTeam):
				return
		self.check()

	def check(self):
		if not Civ4lertsOpt.isShowRefusesToTalkAlert():
			return
		iPlayer = GAME.getActivePlayer()
		CyPlayer = GC.getActivePlayer()
		iTeam = CyPlayer.getTeam()
		CyTeam = GC.getTeam(iTeam)
		refusals = self.refusals[iPlayer]
		aSet = set()
		for iPlayerX in range(GC.getMAX_PC_PLAYERS()):
			if iPlayerX == iPlayer: continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isAlive() or CyPlayerX.isHuman() or CyPlayerX.isMinorCiv(): continue

			iTeamX = CyPlayerX.getTeam()
			if iTeamX == iTeam or not CyTeam.isHasMet(iTeamX) or CyTeam.isAtWar(iTeamX): continue

			if not CyPlayerX.AI_isWillingToTalk(iPlayer):
				aSet.add(CyPlayerX.getID())

		self.display(iPlayer, "TXT_KEY_CIV4LERTS_ON_WILLING_TO_TALK", refusals.difference(aSet))
		self.display(iPlayer, "TXT_KEY_CIV4LERTS_ON_REFUSES_TO_TALK", aSet.difference(refusals))
		self.refusals[iPlayer] = aSet

	def display(self, eActivePlayer, key, players):
		if GAME.getElapsedGameTurns() > 0:
			for ePlayer in players:
				player = GC.getPlayer(ePlayer)
				if player.isAlive():
					message = BugUtil.getText(key, player.getName())
					addMessage(eActivePlayer, message)

	def _reset(self):
		self.refusals = {}
		for i in range(0,GC.getMAX_PC_PLAYERS()):
			self.refusals[i] = set()

class WorstEnemy(AbstractStatefulAlert):
	"""
	Displays an alert when a civilization's worst enemy changes.
	"""
	def __init__(self, eventManager):
		AbstractStatefulAlert.__init__(self, eventManager)
		eventManager.addEventHandler("BeginActivePlayerTurn", self.onBeginActivePlayerTurn)

	def onBeginActivePlayerTurn(self, argsList):
		self.check()

	def onFirstContact(self, argsList):
		eTeam, eRivalTeam = argsList
		self.checkIfIsAnyOrHasMetAllTeams(eTeam, eRivalTeam)

	def onChangeWar(self, argsList):
		#bIsWar = argsList[0]
		eTeam = argsList[1]
		eRivalTeam = argsList[2]
		self.checkIfIsAnyOrHasMetAllTeams(eTeam, eRivalTeam)

	def onCityRazed(self, argsList):
		city, ePlayer = argsList
		self.checkIfIsAnyOrHasMetAllTeams(city.getTeam(), GC.getPlayer(ePlayer).getTeam())

	def onVassalState(self, argsList):
		eMaster = argsList[0]
		eVassal = argsList[1]
		#bVassal = argsList[2]
		self.checkIfIsAnyOrHasMetAllTeams(eMaster, eVassal)

	def checkIfIsAnyOrHasMetAllTeams(self, *eTeams):
		"""
		Calls check() only if the active team is any or has met all of the given teams.
		"""
		iActiveTeam = GC.getGame().getActiveTeam()
		activeTeam = GC.getTeam(iActiveTeam)
		for eTeam in eTeams:
			if eTeam != -1 and iActiveTeam != eTeam and not activeTeam.isHasMet(eTeam):
				return
		self.check()

	def check(self):
		if (not Civ4lertsOpt.isShowWorstEnemyAlert()):
			return
		eActivePlayer = GAME.getActivePlayer()
		iActiveTeam = GC.getGame().getActiveTeam()
		activeTeam = GC.getTeam(iActiveTeam)
		enemies = self.enemies[eActivePlayer]
		newEnemies = AttitudeUtil.getWorstEnemyTeams()
		delayedMessages = {}
		for eTeam, eNewEnemy in newEnemies.iteritems():
			#RevolutionDCM fix
			if eTeam != -1 and activeTeam.isHasMet(eTeam):
				eOldEnemy = enemies[eTeam]
				if eOldEnemy != -1 and not GC.getTeam(eOldEnemy).isAlive():
					eOldEnemy = -1
					enemies[eTeam] = -1
				#RevolutionDCM fix
				if eNewEnemy != -1 and iActiveTeam != eNewEnemy and not activeTeam.isHasMet(eNewEnemy):
					eNewEnemy = -1
				if eOldEnemy != eNewEnemy:
					enemies[eTeam] = eNewEnemy
					if eNewEnemy == -1:
						if eOldEnemy == iActiveTeam:
							message = BugUtil.getText("TXT_KEY_CIV4LERTS_ON_YOU_NO_WORST_ENEMY", GC.getTeam(eTeam).getName())
						else:
							message = BugUtil.getText("TXT_KEY_CIV4LERTS_ON_NO_WORST_ENEMY",
									(GC.getTeam(eTeam).getName(), GC.getTeam(eOldEnemy).getName()))
					elif eOldEnemy == -1:
						message = None # handled below
						if eNewEnemy not in delayedMessages:
							delayedMessages[eNewEnemy] = GC.getTeam(eTeam).getName()
						else:
							delayedMessages[eNewEnemy] += u", " + GC.getTeam(eTeam).getName()
					else:
						if eOldEnemy == iActiveTeam:
							message = BugUtil.getText("TXT_KEY_CIV4LERTS_ON_SWITCH_WORST_ENEMY_FROM_YOU",
									(GC.getTeam(eTeam).getName(), GC.getTeam(eNewEnemy).getName()))
						elif eNewEnemy == iActiveTeam:
							message = BugUtil.getText("TXT_KEY_CIV4LERTS_ON_SWITCH_WORST_ENEMY_TO_YOU",
									(GC.getTeam(eTeam).getName(), GC.getTeam(eOldEnemy).getName()))
						else:
							message = BugUtil.getText("TXT_KEY_CIV4LERTS_ON_SWITCH_WORST_ENEMY",
									(GC.getTeam(eTeam).getName(), GC.getTeam(eNewEnemy).getName(), GC.getTeam(eOldEnemy).getName()))
					if message:
						addMessage(eActivePlayer, message)
		for eEnemy, haters in delayedMessages.iteritems():
			if iActiveTeam == eEnemy:
				message = BugUtil.getText("TXT_KEY_CIV4LERTS_ON_YOU_WORST_ENEMY", haters)
			else:
				message = BugUtil.getText("TXT_KEY_CIV4LERTS_ON_WORST_ENEMY", (haters, GC.getTeam(eEnemy).getName()))
			addMessage(eActivePlayer, message)

	def _reset(self):
		"""
		The enemies dictionary maps all teams to their worst enemy.
		It will hold -1 for any team or enemy the active team hasn't met.
		"""
		self.enemies = {}
		for i in range(0,GC.getMAX_PC_PLAYERS()):
			self.enemies[i] = [-1] * GC.getMAX_TEAMS()
