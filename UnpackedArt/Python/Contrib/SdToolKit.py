## sdToolKit by Stone-D (Laga Mahesa)
## Copyright Laga Mahesa 2005
##
## laga@tbi.co.id
## lmahesa@(yahoo|hotmail|gmail).com
##
## Version 1.22
##
## Rewritten to use BugData by EmperorFool
## Merged with SdToolKitCustom by AIAndy (usage of both was conflicting with BugData)

from CvPythonExtensions import *
import BugData
import cPickle

#-=-=-=-=-=-=-=-= SD-DATA-STORAGE =-=-=-=-=-=-=-=-=-#
# Every variable is a string, except for the actual
# value you want to store, which can be anything.

#----------------- MOD FUNCTIONS -------------------#

# sdEntityInit('MyModName', 'UniqueName', Template_dictionary)
# Initializes a unique data entity (city, unit, plot).
def sdEntityInit(ModID, entity, eTable):
	table = BugData.getTable(ModID, entity)
	table.setData(eTable)
	return 0

# sdEntityWipe('MyModName', 'UniqueName')
# Removes an entity that has been previously initialized by sdEntityInit.
# Returns int 0 on failure, int 1 on success.
def sdEntityWipe(ModID, entity):
	return BugData.deleteTable(ModID, entity)

# sdEntityExists('MyModName', 'UniqueName')
# Checks whether or not an entity has been initialized by sdEntityInit.
# Returns bool False on failure, bool True on success.
def sdEntityExists(ModID, entity):
	return BugData.hasTable(ModID, entity)

# sdGetVal('MyModName', 'UniqueName', 'VariableName')
# Fetches a specific variable's value from the entity's data set.
def sdGetVal(ModID, entity, var):
	table = BugData.findTable(ModID, entity)
	if table:
		if var in table:
			return table[var]
	return None

# sdSetVal('MyModName', 'UniqueName', 'VariableName', any_value)
# Stores a specific variable's value within the entity's data set.
# Returns bool False on failure, bool True on success.
def sdSetVal(ModID, entity, var, val):
	table = BugData.findTable(ModID, entity)
	if table:
		table[var] = val
		return True
	return False

# sdDelVal('MyModName', 'UniqueName', 'VariableName')
# Removes a specific variable from the entity's data set.
# Returns bool False on failure, bool True on success.
def sdDelVal(ModID, entity, var):
	table = BugData.findTable(ModID, entity)
	if table and var in table:
		del table[var]
		return True
	return False

# sdGetGlobal('MyModName', 'GlobalVariableName')
# Fetches a specific variable's value from the mod's global data set.
def sdGetGlobal(ModID, var):
	table = BugData.findTable(ModID, 'Global')
	if table and var in table:
		return table[var]
	return None

# sdSetGlobal('MyModName', 'GlobalVariableName', any_value)
# Stores a specific variable's value within the mod's global data set.
def sdSetGlobal(ModID, var, val):
	BugData.getTable(ModID, 'Global')[var] = val

# sdDelGlobal('MyModName', 'GlobalVariableName')
# Removes a specific variable from the mod's global data set.
# Returns bool False on failure, bool True on success.
def sdDelGlobal(ModID, var):
	table = BugData.findTable(ModID, 'Global')
	if table and var in table:
		del table[var]
		return True
	return False


## Modification by Teg Navanis. While SD-DATA-STORAGE stores
## values in the GameInstance - scriptdata, these functions can be used to store data
## in the scriptdata of an object (for instance a unit, a city or a plot)

## Further modifications by jdog5000

#-=-=-=-=-=-=-=-= SD-OBJECT-DATA-STORAGE =-=-=-=-=-=-=-=-=-#
# Every variable is a string, except for 'object' and the actual
# value you want to store, which can be anything.
# object can be one of the following:
# - CyCity object
# - CyGame object
# - CyPlayer object
# - CyPlot object
# - CyUnit object
# - PyCity object

# AIAndy: Using the CyGame object is now redirected to the functions above to use BugData

#--------------- INTERNAL USE ONLY -----------------#

# Loads previously initialized data from the central reservoir. If no data is found, init it.
def sdLoad(object):

	cyTable = {}
	if object:
		temp = object.getScriptData()
		if temp:
			cyTable = cPickle.loads(temp)

	return cyTable

# Loads previously initialized data from the central reservoir. If no data is found, init it.
def sdObjectGetDict(ModID, object):
	cyTable = sdLoad(object)
	if ModID in cyTable:
		return cyTable[ModID]
	return {}

# Loads previously initialized data from the central reservoir. If no data is found, init it.
def sdObjectSetDict(ModID, object, VarDictionary):
	cyTable = sdLoad(object)
	cyTable[ModID] = VarDictionary
	object.setScriptData(cPickle.dumps(cyTable))


#----------------- OBJECT FUNCTIONS -------------------#

# sdObjectInit ('MyModName', object, Template_dictionary)
# Fetches a specific variable's value from the object's data set.
def sdObjectInit(ModID, object, VarDictionary):
	if isinstance(object, CyGame):
		if not BugData.hasTable(ModID, 'Game'):
			sdEntityInit(ModID, 'Game', VarDictionary)
			return 1
	else:
		cyTable = sdLoad(object)
		if ModID not in cyTable:
			cyTable[ModID] = VarDictionary
			object.setScriptData(cPickle.dumps(cyTable))
			return 1


# sdObjectWipe('MyModName', object)
# Removes an entity that has been previously initialized by sdObjectInit.
# Returns False on failure, True on success.
def sdObjectWipe(ModID, object):
	if isinstance(object, CyGame):
		return sdEntityWipe(ModID, 'Game')

	cyTable = sdLoad(object)
	if ModID in cyTable:
		del cyTable[ModID]
		object.setScriptData(cPickle.dumps(cyTable))
		return True
	return False


# sdObjectExists('MyModName', object)
# Checks whether or not an object has been initialized by sdObjectInit.
# Returns bool False on failure, bool True on success.
def sdObjectExists(ModID, object):
	if isinstance(object, CyGame):
		if BugData.hasTable(ModID, 'Game'):
			return True
	else:
		cyTable = sdLoad(object)
		if ModID in cyTable:
			return True
	return False


# sdObjectGetVal('MyModName', object, 'VariableName')
# Fetches a specific variable's value from the object's data set.
## Modded by jdog5000: returns None if not found
def sdObjectGetVal(ModID, object, var):
	if isinstance(object, CyGame):
		return sdGetVal(ModID, 'Game', var)
	cyTable = sdLoad(object)
	if ModID in cyTable:
		mTable = cyTable[ModID]
		if var in mTable:
			return mTable[var]
	return None

# sdObjectSetVal('MyModName', object, 'VariableName', any_value)
# Stores a specific variable's value within the object's data set.
# Returns bool False on failure, bool True on success.
## Modded by jdog5000 to allow creation of new dict elements
def sdObjectSetVal(ModID, object, var, val):
	if isinstance(object, CyGame):
		return sdSetVal(ModID, 'Game', var, val)

	cyTable = sdLoad(object)
	if ModID in cyTable:
		mTable = cyTable[ModID]
		if var in mTable:
			mTable[var] = val
			object.setScriptData(cPickle.dumps(cyTable))
			return True
		else:
			mTable[var] = val
			object.setScriptData(cPickle.dumps(cyTable))
			return True
	return False

# sdObjectChangeVal('MyModName', object, 'VariableName', change_in_value)
# Updates an existing variable's value within the object's data set.
# Returns bool False on failure, bool True on success.
def sdObjectChangeVal(ModID, object, var, delta):
	if isinstance(object, CyGame):
		table = BugData.findTable(ModID, 'Game')
		if table and var in table:
			table[var] += delta
			return True
		return False

	cyTable = sdLoad(object)
	if ModID in cyTable:
		mTable = cyTable[ModID]

		prevVal = sdObjectGetVal(ModID, object, var)
		if not prevVal == None and var in mTable:
			mTable[var] = prevVal + delta
			object.setScriptData(cPickle.dumps(cyTable))
			return True
	return False

# sdObjectUpdateVal('MyModName', object, 'VariableName', any_value)
# Updates an existing variable's value within the object's data set.
# Returns bool False on failure, bool True on success.
def sdObjectUpdateVal(ModID, object, var, val):

	if isinstance(object, CyGame):
		table = BugData.findTable(ModID, 'Game')
		if table and var in table:
			table[var] = delta
			return True
		return False

	cyTable = sdLoad(object)
	if ModID in cyTable:
		mTable = cyTable[ModID]
		if var in mTable:
			mTable[var] = val
			object.setScriptData(cPickle.dumps(cyTable))
			return True
	return False