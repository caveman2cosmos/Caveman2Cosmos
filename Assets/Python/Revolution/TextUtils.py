# Text utilities
#
# by jdog5000
# Version 1.0

from CvPythonExtensions import *
import PyHelpers

# globals
gc = CyGlobalContext()
game = CyGame()
localText = CyTranslator()
PyPlayer = PyHelpers.PyPlayer

def getTextItemList( textList, pre = '', sep = ', ', second = '', penUlt = '', post = '' ) :

	str = pre
	#RevolutionDCM - cannot execute this function call within the parameter list
	penUlt = localText.getText("TXT_KEY_REV_AND",()) + ' '

	if( len(textList) == 0 ) :
		return str.strip()

	str += textList[0]

	if( len(textList) > 1 ) :
		str += second
		if( second == '' ) :
			str += sep

		for text in textList[1:-1] :
			str += text + sep

		if( len(textList) > 2 or second == '' ) :
			str += penUlt
		str += textList[-1]

	str += post

	return str

def getCityTextList( cityList, bPreCity = False, bPreCitizens = False, sep = ', ', second = '', penUlt = '', bPostIs = False ) :

	textList = list()
	#RevolutionDCM - cannot execute this function call within the parameter list
	penUlt = localText.getText("TXT_KEY_REV_AND",()) + ' '

	for pCity in cityList :
		textList.append(pCity.getName())

	pre = ''
	if( bPreCity ) :
		if( len(cityList) > 1 and second == '' ) :
			pre = localText.getText("TXT_KEY_REV_CITIES_OF",()) + ' '
		else :
			pre = localText.getText("TXT_KEY_REV_CITY_OF",()) + ' '
	elif( bPreCitizens ) :
		pre = localText.getText("TXT_KEY_REV_CITIZENS_OF",()) + ' '

	post = ''
	if( bPostIs ) :
		if( (len(cityList) > 1 and second == '') or bPreCitizens ) :
			post += ' ' + localText.getText("TXT_KEY_REV_ARE",())
		else :
			post += ' ' + localText.getText("TXT_KEY_REV_IS",())
			if( not second == '' ) :
				post = sep.strip() + post

	return getTextItemList(textList, pre = pre, sep = sep, second = second, penUlt = penUlt, post = post)

def testCityTextList( iPlayer ):

	cityList = gc.getPlayer(iPlayer).cities()

	str = getCityTextList(cityList)
	CyInterface().addImmediateMessage(str,"")
	print str

	str = getCityTextList(cityList, bPreCity = True, second = localText.getText("TXT_KEY_REV_ALONG_WITH",()) + ' ', bPostIs = True)
	CyInterface().addImmediateMessage(str,"")
	print str

	str = getCityTextList(cityList, bPreCitizens = True, penUlt = '', bPostIs = True)
	CyInterface().addImmediateMessage(str,"")
	print str

	str = getCityTextList(cityList, bPreCity = True, bPostIs = True)
	CyInterface().addImmediateMessage(str,"")
	print str