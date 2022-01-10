# Text utilities
#
# by jdog5000
# Version 1.0

from CvPythonExtensions import CyTranslator

localText = CyTranslator()

def getTextItemList(textList, pre = '', sep = ', ', second = '', penUlt = '', post = ''):

	str = pre
	penUlt = localText.getText("TXT_KEY_REV_AND",()) + ' '

	if not textList:
		return str.strip()

	str += textList[0]

	if len(textList) > 1:
		str += second
		if second == '':
			str += sep

		for text in textList[1:-1] :
			str += text + sep

		if len(textList) > 2 or second == '':
			str += penUlt
		str += textList[-1]

	return str + post

def getCityTextList(cityList, bPreCity = False, bPreCitizens = False, sep = ', ', second = '', penUlt = '', bPostIs = False):

	textList = []
	penUlt = localText.getText("TXT_KEY_REV_AND",()) + ' '

	for pCity in cityList:
		textList.append(pCity.getName())

	pre = ''
	if bPreCity:
		if len(cityList) > 1 and second == '':
			pre = localText.getText("TXT_KEY_REV_CITIES_OF",()) + ' '
		else: pre = localText.getText("TXT_KEY_REV_CITY_OF",()) + ' '

	elif bPreCitizens:
		pre = localText.getText("TXT_KEY_REV_CITIZENS_OF",()) + ' '

	post = ''
	if bPostIs:
		if bPreCitizens or len(cityList) > 1 and second == '':
			post += ' ' + localText.getText("TXT_KEY_REV_ARE",())
		else:
			post += ' ' + localText.getText("TXT_KEY_REV_IS",())
			if second != '':
				post = sep.strip() + post

	return getTextItemList(textList, pre = pre, sep = sep, second = second, penUlt = penUlt, post = post)
