## UnitGrouper
##
## Builds groups of units for use in reporting or screens.
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *
import BugUtil

gc = CyGlobalContext()

# Base grouping classes

class Grouper:
	"""
	Holds all Grouping definitions.
	"""
	def __init__(self):
		self.groupings = []
		self.groupingsByKey = {}

	def _addGrouping(self, grouping):
		grouping.index = len(self.groupings)
		self.groupings.append(grouping)
		self.groupingsByKey[grouping.key] = grouping

	def getGrouping(self, key):
		if key in self.groupingsByKey:
			return self.groupingsByKey[key]
		else:
			return None

	def __getitem__(self, key):
		if isinstance(key, int):
			return self.groupings[key]
		else:
			return self.groupingsByKey[key]

	def __iter__(self):
		return self.groupings.__iter__()

	def iterkeys(self):
		return self.groupingsByKey.iterkeys()

	def itervalues(self):
		return self.groupingsByKey.itervalues()

	def iteritems(self):
		return self.groupingsByKey.iteritems()

class Grouping:
	"""
	Applies a formula to place units into groups.

	key: used for sorting groupings; must be in the range [0, 999] inclusive
	title: used to display the group
	"""
	def __init__(self, key, title):
		self.index = None
		self.key = key
		if title.startswith("TXT_KEY_"):
			self.title = BugUtil.getPlainText(title)
		else:
			self.title = title
		self.groups = {}

	def _addGroup(self, group):
		self.groups[group.key] = group

	def calcGroupKeys(self, unit, player, team):
		return None

class Group:
	"""
	Represents a single group value within a grouping.

	key: used for sorting groups; must be in the range [0, 999] inclusive
	title: used to display the group
	"""
	def __init__(self, grouping, key, title):
		self.grouping = grouping
		self.key = key
		if title.startswith("TXT_KEY_"):
			self.title = BugUtil.getPlainText(title)
		else:
			self.title = title

	def getTitle(self):
		return self.title


# Grouping definitions

class UnitTypeGrouping(Grouping):
	"""
	Groups units by their unit type.
	Ex: Warrior, Maceman, Panzer
	"""
	def __init__(self):
		Grouping.__init__(self, "type", "TXT_KEY_UNITGROUPER_TYPE_GROUPING")

		for i in range(gc.getNumUnitInfos()):
			info = gc.getUnitInfo(i)
			if info:
				self._addGroup(Group(self, i, info.getDescription()))

	def calcGroupKeys(self, unit, player, team):
		return (unit.getUnitType(),)

class UnitCombatGrouping(Grouping):
	"""
	Groups units by their combat type.
	Ex: None, Melee, Gunpowder, Naval
	"""
	def __init__(self):
		Grouping.__init__(self, "combat", "TXT_KEY_UNITGROUPER_COMBAT_GROUPING")
		self.NONE = 0

		self._addGroup(Group(self, self.NONE, "TXT_KEY_UNITGROUPER_COMBAT_GROUP_NONE"))
		for i in range(gc.getNumUnitCombatInfos()):
			info = gc.getUnitCombatInfo(i)
			if info:
				self._addGroup(Group(self, i + 1, info.getDescription()))

	def calcGroupKeys(self, unit, player, team):
		return (gc.getUnitInfo(unit.getUnitType()).getUnitCombatType() + 1,)

class LevelGrouping(Grouping):
	"""
	Groups units by their level, 1 to MAX_LEVEL (50).
	Units over level MAX_LEVEL are put into the MAX_LEVEL group.
	"""
	def __init__(self):
		Grouping.__init__(self, "level", "TXT_KEY_UNITGROUPER_LEVEL_GROUPING")

		self.MAX_LEVEL = 50
		for i in range(self.MAX_LEVEL):
			self._addGroup(Group(self, i, BugUtil.getText("TXT_KEY_UNITGROUPER_LEVEL_GROUP", (str(i),))))
		self._addGroup(Group(self, self.MAX_LEVEL, BugUtil.getText("TXT_KEY_UNITGROUPER_LEVEL_GROUP", ("%d+" % self.MAX_LEVEL,))))

	def calcGroupKeys(self, unit, player, team):
		return (max(0, min(unit.getLevel(), self.MAX_LEVEL)),)

class PromotionGrouping(Grouping):
	"""
	Groups units by their promotions.
	Ex: Combat 1, Cover, Tactics
	"""
	def __init__(self):
		Grouping.__init__(self, "promo", "TXT_KEY_UNITGROUPER_PROMOTION_GROUPING")

		self.NONE = 0
		self.NO_PROMOS = (0,)
		self._addGroup(Group(self, self.NONE, "TXT_KEY_UNITGROUPER_PROMOTION_GROUP_NONE"))
		for i in range(gc.getNumPromotionInfos()):
			info = gc.getPromotionInfo(i)
			if info:
				self._addGroup(Group(self, i + 1, u'<img=%s size=16></img> %s' % (info.getButton(), info.getDescription())))

	def calcGroupKeys(self, unit, player, team):
		promos = []
		for iPromo in range(gc.getNumPromotionInfos()):
			if unit.isHasPromotion(iPromo):
				promos.append(iPromo + 1)
		if not promos:
			promos = self.NO_PROMOS
		return promos

class LocationGrouping(Grouping):
	"""
	Groups units by their location on the map.
	Ex: Domestic City, Friendly City, Enemy Territory
	"""
	def __init__(self):
		Grouping.__init__(self, "loc", "TXT_KEY_UNITGROUPER_LOCATION_GROUPING")
		(
			self.DOMESTIC_CITY,
			self.DOMESTIC_TERRITORY,
			self.TEAM_CITY,
			self.TEAM_TERRITORY,
			self.FRIENDLY_CITY,
			self.FRIENDLY_TERRITORY,
			self.NEUTRAL_TERRITORY,
			self.ENEMY_TERRITORY,
			self.BARBARIAN_TERRITORY
		) = range(9)

		self._addGroup(Group(self, self.DOMESTIC_CITY, "TXT_KEY_UNITGROUPER_LOCATION_GROUP_DOMESTIC_CITY"))
		self._addGroup(Group(self, self.DOMESTIC_TERRITORY, "TXT_KEY_UNITGROUPER_LOCATION_GROUP_DOMESTIC_TERRITORY"))
		self._addGroup(Group(self, self.TEAM_CITY, "TXT_KEY_UNITGROUPER_LOCATION_GROUP_TEAM_CITY"))
		self._addGroup(Group(self, self.TEAM_TERRITORY, "TXT_KEY_UNITGROUPER_LOCATION_GROUP_TEAM_TERRITORY"))
		self._addGroup(Group(self, self.FRIENDLY_CITY, "TXT_KEY_UNITGROUPER_LOCATION_GROUP_FRIENDLY_CITY"))
		self._addGroup(Group(self, self.FRIENDLY_TERRITORY, "TXT_KEY_UNITGROUPER_LOCATION_GROUP_FRIENDLY_TERRITORY"))
		self._addGroup(Group(self, self.NEUTRAL_TERRITORY, "TXT_KEY_UNITGROUPER_LOCATION_GROUP_NEUTRAL_TERRITORY"))
		self._addGroup(Group(self, self.ENEMY_TERRITORY, "TXT_KEY_UNITGROUPER_LOCATION_GROUP_ENEMY_TERRITORY"))
		self._addGroup(Group(self, self.BARBARIAN_TERRITORY, "TXT_KEY_UNITGROUPER_LOCATION_GROUP_BARBARIAN_TERRITORY"))

	def calcGroupKeys(self, unit, player, team):
		plot = unit.plot()
		if not plot or plot.isNone():
			return None
		if plot.isNPC():
			return (self.BARBARIAN_TERRITORY,)
		teamId = team.getID()
		ownerId = plot.getRevealedOwner(teamId, False)
		if ownerId == -1:
			return (self.NEUTRAL_TERRITORY,)
		elif ownerId == player.getID():
			if plot.isCity():
				return (self.DOMESTIC_CITY,)
			else:
				return (self.DOMESTIC_TERRITORY,)
		else:
			owner = gc.getPlayer(ownerId)
			ownerTeamId = owner.getTeam()
			if ownerTeamId == teamId:
				if plot.isCity():
					return (self.TEAM_CITY,)
				else:
					return (self.TEAM_TERRITORY,)
			elif team.isAtWarWith(ownerTeamId):
				return (self.ENEMY_TERRITORY,)
			else:
				if plot.isCity():
					return (self.FRIENDLY_CITY,)
				else:
					return (self.FRIENDLY_TERRITORY,)

class OrderGrouping(Grouping):
	"""
	Groups units by their current order/action.
	Ex: Fortify, Go To, Blockade
	"""
	def __init__(self):
		Grouping.__init__(self, "order", "TXT_KEY_UNITGROUPER_ORDER_GROUPING")

		self._addGroup(Group(self, 0, "TXT_KEY_UNITGROUPER_ORDER_GROUP_NONE"))
		self._addGroup(Group(self, 1, "TXT_KEY_UNITGROUPER_ORDER_GROUP_SKIP"))
		self._addGroup(Group(self, 2, "TXT_KEY_UNITGROUPER_ORDER_GROUP_SLEEP"))
		self._addGroup(Group(self, 3, "TXT_KEY_UNITGROUPER_ORDER_GROUP_FORTIFY"))
		self._addGroup(Group(self, 4, "TXT_KEY_UNITGROUPER_ORDER_GROUP_HEAL"))
		self._addGroup(Group(self, 5, "TXT_KEY_UNITGROUPER_ORDER_GROUP_SENTRY"))
		self._addGroup(Group(self, 6, "TXT_KEY_UNITGROUPER_ORDER_GROUP_INTERCEPT"))
		self._addGroup(Group(self, 7, "TXT_KEY_UNITGROUPER_ORDER_GROUP_PATROL"))
		self._addGroup(Group(self, 8, "TXT_KEY_UNITGROUPER_ORDER_GROUP_PLUNDER"))
		self._addGroup(Group(self, 9, "TXT_KEY_UNITGROUPER_ORDER_GROUP_BUILD"))
		self._addGroup(Group(self, 10, "TXT_KEY_UNITGROUPER_ORDER_GROUP_CONSTRUCT"))
		self._addGroup(Group(self, 11, "TXT_KEY_UNITGROUPER_ORDER_GROUP_GOTO"))
		self._addGroup(Group(self, 12, "TXT_KEY_UNITGROUPER_ORDER_GROUP_EXPLORE"))
		self._addGroup(Group(self, 13, "TXT_KEY_UNITGROUPER_ORDER_GROUP_AUTO_BUILD"))
		self._addGroup(Group(self, 14, "TXT_KEY_UNITGROUPER_ORDER_GROUP_AUTO_NETWORK"))
		self._addGroup(Group(self, 15, "TXT_KEY_UNITGROUPER_ORDER_GROUP_AUTO_CITY"))
		self._addGroup(Group(self, 16, "TXT_KEY_UNITGROUPER_ORDER_GROUP_AUTO_RELIGION"))
		self._addGroup(Group(self, 17, "TXT_KEY_UNITGROUPER_ORDER_GROUP_PILLAGESETTINGS"))
		self._addGroup(Group(self, 18, "TXT_KEY_UNITGROUPER_ORDER_GROUP_HUNTSETTINGS"))
		self._addGroup(Group(self, 19, "TXT_KEY_UNITGROUPER_ORDER_GROUP_DEFENSESETTINGS"))
		self._addGroup(Group(self, 20, "TXT_KEY_UNITGROUPER_ORDER_GROUP_PATROLSETTINGS"))
		self._addGroup(Group(self, 21, "TXT_KEY_UNITGROUPER_ORDER_GROUP_PIRATESETTINGS"))
		self._addGroup(Group(self, 22, "TXT_KEY_UNITGROUPER_ORDER_GROUP_AIRSTRIKESETTINGS"))
		self._addGroup(Group(self, 23, "TXT_KEY_UNITGROUPER_ORDER_GROUP_AIRBOMBSETTINGS"))
		self._addGroup(Group(self, 24, "TXT_KEY_UNITGROUPER_ORDER_GROUP_AIRUNITCANDEFEND"))
		self._addGroup(Group(self, 25, "TXT_KEY_UNITGROUPER_ORDER_GROUP_PROTECTSETTINGS"))
		self._addGroup(Group(self, 26, "TXT_KEY_UNITGROUPER_ORDER_GROUP_OTHER"))

		self._AutomationMap = {
			AutomateTypes.AUTOMATE_CITY	: 15,
			AutomateTypes.AUTOMATE_HUNT	: 18,

			AutomateTypes.AUTOMATE_EXPLORE	: 12,
			AutomateTypes.AUTOMATE_BUILD	: 13,
			AutomateTypes.AUTOMATE_NETWORK	: 14,
			AutomateTypes.AUTOMATE_RELIGION	: 16,
			AutomateTypes.AUTOMATE_PILLAGE	: 17,
			AutomateTypes.AUTOMATE_PIRATE	: 21,
			AutomateTypes.AUTOMATE_AIRBOMB	: 23,
			AutomateTypes.AUTOMATE_SHADOW	: 25,

			AutomateTypes.AUTOMATE_CITY_DEFENSE	: 19,
			AutomateTypes.AUTOMATE_BORDER_PATROL: 20,
			AutomateTypes.AUTOMATE_AIRSTRIKE	: 22,
			AutomateTypes.AUTOMATE_AIR_RECON	: 24
		}
		self._ActivityMap = {
			ActivityTypes.ACTIVITY_HEAL	: 4,
			ActivityTypes.ACTIVITY_HOLD	: 1,

			ActivityTypes.ACTIVITY_AWAKE	: 0,
			ActivityTypes.ACTIVITY_INTERCEPT: 6,
			ActivityTypes.ACTIVITY_PATROL	: 7,
			ActivityTypes.ACTIVITY_PLUNDER	: 8,
			ActivityTypes.ACTIVITY_SENTRY	: 5,

			ActivityTypes.ACTIVITY_SENTRY_WHILE_HEAL	: 4,
			ActivityTypes.ACTIVITY_SENTRY_LAND_UNITS	: 5,
			ActivityTypes.ACTIVITY_SENTRY_NAVAL_UNITS	: 5
		}

	def calcGroupKeys(self, unit, player, team):
		return (self._getOrder(unit),)

	def _getOrder(self, unit):
		group = unit.getGroup()

		iAutomation = group.getAutomateType()
		if iAutomation in self._AutomationMap:
			return self._AutomationMap[iAutomation]

		eActivityType = group.getActivityType()
		if eActivityType in self._ActivityMap:
			return self._ActivityMap[eActivityType]

		if group.getLengthMissionQueue() > 0:
			eMissionType = group.getMissionType(0)
			if eMissionType == MissionTypes.MISSION_BUILD:
				return 9 # Build
			if eMissionType in (MissionTypes.MISSION_MOVE_TO, MissionTypes.MISSION_MOVE_TO_UNIT, MissionTypes.MISSION_MOVE_TO_SENTRY):
				return 11 # GoTo
		elif unit.isWaiting():
			if unit.isFortifyable():
				return 3 # Fortify
			return 2 # Sleep
		return 26 # Other


class AutoUpgradeGrouping(Grouping):
	"""
	Groups units by their AUA automated upgrade status.
	Ex: AUTOMATE_UPGRADING
	"""
	def __init__(self):
		Grouping.__init__(self, "autoupgrading", "TXT_KEY_UNITGROUPER_AUTO_UPGRADE_GROUPING")
		(
			self.ORDER_AUTO_UPGRADING,
			self.ORDER_NONE,
		) = range(2)
		self._addGroup(Group(self, self.ORDER_AUTO_UPGRADING, "TXT_KEY_UNITGROUPER_ORDER_GROUP_UPGRADINGSETTINGS"))
		self._addGroup(Group(self, self.ORDER_NONE, "TXT_KEY_UNITGROUPER_ORDER_GROUP_NONE"))

	def calcGroupKeys(self, unit, player, team):
		if unit.isAutoUpgrading():
			return (self.ORDER_AUTO_UPGRADING,)
		return (self.ORDER_NONE,)

class AutoPromoteGrouping(Grouping):
	"""
	Groups units by their AUA automated promotion status.
	Ex: AUTOMATE_PROMOTIONS
	"""
	def __init__(self):
		Grouping.__init__(self, "autopromotion", "TXT_KEY_UNITGROUPER_AUTO_PROMOTION_GROUPING")
		(
			self.ORDER_AUTO_PROMOTIONS,
			self.ORDER_NONE,
		) = range(2)
		self._addGroup(Group(self, self.ORDER_AUTO_PROMOTIONS, "TXT_KEY_UNITGROUPER_ORDER_GROUP_PROMOTESETTINGS"))
		self._addGroup(Group(self, self.ORDER_NONE, "TXT_KEY_UNITGROUPER_ORDER_GROUP_NONE"))

	def calcGroupKeys(self, unit, player, team):
		if unit.isAutoPromoting():
			return (self.ORDER_AUTO_PROMOTIONS,)
		return (self.ORDER_NONE,)


class StandardGrouper(Grouper):
	def __init__(self):
		Grouper.__init__(self)

		self._addGrouping(UnitTypeGrouping())
		self._addGrouping(UnitCombatGrouping())
		self._addGrouping(LevelGrouping())
		self._addGrouping(PromotionGrouping())
		self._addGrouping(LocationGrouping())
		self._addGrouping(OrderGrouping())
		self._addGrouping(AutoUpgradeGrouping())
		self._addGrouping(AutoPromoteGrouping())

# Classes for tracking stats about groups and units

class GrouperStats:
	"""
	Holds stats for a set of groupings.
	"""
	def __init__(self, grouper):
		self.grouper = grouper
		self.groupings = {}

		for grouping in self.grouper.groupings:
			self._addGrouping(GroupingStats(grouping))

	def _addGrouping(self, grouping):
		self.groupings[grouping.grouping.key] = grouping

	def processUnit(self, player, team, unit):
		stats = UnitStats(unit.getOwner(), unit.getID(), unit)
		for grouping in self.groupings.itervalues():
			grouping._processUnit(player, team, stats)
		return stats

	def getGrouping(self, key):
		if key in self.groupings:
			return self.groupings[key]
		else:
			return None

	def itergroupings(self):
		return self.groupings.itervalues()

class GroupingStats:
	"""
	Holds stats for a grouping.
	"""
	def __init__(self, grouping):
		self.grouping = grouping
		self.groups = {}

		for group in self.grouping.groups.itervalues():
			self._addGroup(GroupStats(group))

	def _addGroup(self, group):
		self.groups[group.group.key] = group

	def _processUnit(self, player, team, unitStats):
		keys = self.grouping.calcGroupKeys(unitStats.unit, player, team)
		for key in keys:
			self.groups[key]._addUnit(unitStats)

	def itergroups(self):
		return self.groups.itervalues()

class GroupStats:
	"""
	Holds stats for a group of units.
	"""
	def __init__(self, group):
		self.group = group
		self.units = set()

	def _addUnit(self, unitStats):
		self.units.add(unitStats)

	def title(self):
		return self.group.title

	def size(self):
		return len(self.units)

	def isEmpty(self):
		return self.size() == 0

class UnitStats:
	"""
	Holds stats about a single unit.
	"""
	def __init__(self, playerId, unitId, unit):
		self.key = (playerId, unitId)
		self.unit = unit

	def __hash__(self):
		return hash(self.key)

	def __eq__(self, other):
		return self.key == other.key
