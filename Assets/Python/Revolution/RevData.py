# Interface to SdToolKit for Revolution Mod
#
# by jdog5000
# Version 0.2

# All  changed SDToolKitCustom to SDKToolKit as suggested by dusckr87 and questdog

import CvPythonExtensions

import SdToolKit
import RevDefs

#   Loads previously initialized data for Revolution mod from the central reservoir. If no data is found, init it.
def revLoad(object):
	cyTable = SdToolKit.sdLoad(object)

	if 'Revolution' in cyTable:
		return cyTable['Revolution']
	return {}

#----------------- OBJECT FUNCTIONS -------------------#

#   revObjectInit ( object, Template_dictionary )
#   Initializes objects revolution member to the template dictionary.
def revObjectInit(object, VarDictionary = {}):
	return SdToolKit.sdObjectInit('Revolution', object, VarDictionary)

#   revObjectWipe( object )
#   Removes an entity that has been previously initialized by sdObjectInit.
#   Returns False on failure, True on success.
def revObjectWipe(object):
	return SdToolKit.sdObjectWipe('Revolution', object)

#   revObjectExists( object )
#   Checks whether or not an object has been initialized by sdObjectInit.
#   Returns bool False on failure, bool True on success.
def revObjectExists(object):
	return SdToolKit.sdObjectExists('Revolution', object)

#   revObjectGetVal( object, 'VariableName' )
#   Fetches a specific variable's value from the object's data set.
def revObjectGetVal(object, var):

	if not revObjectExists(object):
		if isinstance(object, CvPythonExtensions.CyPlayer):
			print "RevData:  Initializing player object"
			initPlayer(object)
		elif isinstance(object, CvPythonExtensions.CyCity):
			print "RevData:  Initializing city object"
			initCity(object)
		else:
			# SDTK will fail
			print "ERROR:  Not recognized: ", object

	#TODO: remove this once all have been put into data structure defs
	if isinstance(object, CvPythonExtensions.CyPlayer) and not var in RevDefs.playerData.keys():
		print "WARNING:  Unrecognized player variable ", var
	elif isinstance(object, CvPythonExtensions.CyCity) and not var in RevDefs.cityData.keys():
		print "WARNING:  Unrecognized city variable ", var

	return SdToolKit.sdObjectGetVal('Revolution', object, var)

#   revObjectSetVal( object, 'VariableName', any_value )
#   Stores a specific variable's value within the object's data set.
#   Returns bool False on failure, bool True on success.
def revObjectSetVal(object, var, val):

	if not revObjectExists(object):
		if isinstance(object, CvPythonExtensions.CyPlayer):
			print "RevData:  Initializing player object"
			initPlayer(object)
		elif isinstance(object, CvPythonExtensions.CyCity):
			print "RevData:  Initializing city object"
			initCity(object)
		else:
			# SDTK will fail
			print "ERROR:  Not recognized: ", object

	return SdToolKit.sdObjectSetVal( 'Revolution', object, var, val )

#   revObjectChangeVal( object, 'VariableName', change_in_value )
#   Updates an existing variable's value within the object's data set.
#   Returns bool False on failure, bool True on success.
def revObjectChangeVal( object, var, delta ):

	return SdToolKit.sdObjectChangeVal( 'Revolution', object, var, delta )

#   revObjectUpdateVal( object, 'VariableName', any_value )
#   Updates an existing variable's value within the object's data set.
#   Returns bool False on failure, bool True on success.
def revObjectUpdateVal( object, var, val ):

	return SdToolKit.sdObjectUpdateVal( 'Revolution', object, var, val )


def initCity(pCity):

	if revObjectExists(pCity):
		revObjectWipe(pCity)

	if revObjectInit(pCity, RevDefs.cityData):
		setCityVal(pCity, 'RevIdxHistory', RevDefs.initRevIdxHistory())
		return True
	return False

def initPlayer(pPlayer):

	if revObjectExists(pPlayer):
		revObjectWipe(pPlayer)

	return revObjectInit(pPlayer, RevDefs.playerData)

# ----------- Functions for cities ------------------

def getCityVal(pCity, var):

	val = revObjectGetVal(pCity, var)
	if val is None and var not in RevDefs.cityData.keys():
		initCity(pCity)
		print "Error!  Var ", var, " is not a valid cityData variable"
		assert(False)

	if var == 'RevIdxHistory':
		if val == None:
			revIdxHist = RevDefs.initRevIdxHistory()
			setCityVal(pCity, 'RevIdxHistory', revIdxHist)
			return revIdxHist
		if len(val.keys()) < len(RevDefs.revIdxHistKeyList):
			for key in RevDefs.revIdxHistKeyList:
				if not val.has_key(key):
					print "Warning!  RevIdxHistory missing key ", key, ", initializing it"
					val[key] = [0]

	return val

def setCityVal(pCity, var, val):

	success = revObjectSetVal(pCity, var, val)
	if not success:
		initCity(pCity)
		return revObjectSetVal(pCity, var, val)

	return success

def updateCityVal(pCity, var, val):

	success = revObjectUpdateVal(pCity, var, val)
	if not success:
		initCity(pCity)
		return revObjectUpdateVal(pCity, var, val)

	return success

def changeCityVal(pCity, var, delta):

	if var == "RevolutionIndex" and delta != int(delta):
		assert(False)

	success = revObjectChangeVal(pCity, var, delta)
	if not success:
		initCity(pCity)
		return revObjectChangeVal(pCity, var, delta)

	return success