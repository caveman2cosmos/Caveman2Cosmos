
from CvPythonExtensions import *

CyIF = CyInterface()

############
# Encoding #
############
# if the string is non unicode, convert it to unicode by decoding it using utf-8
def convertToUnicode(s):
	if isinstance(s, str):
		return s.decode("iso8859")
	return s

# if the string is unicode, convert it to str by encoding it using utf-8
def convertToStr(txt):
	if isinstance(txt, unicode):
		i = 0
		length = len(txt)
		while i < length:
			ordinal = ord(txt[i])
			if ordinal > 255:
				txt = txt[:i] + '?' + txt[i+1:]
			i += 1
		 # Toffer - "iso8859" = "latin-1". Tried UTF-8 here, caused problem for german characters like "ß".
		return txt.encode("iso8859")
	return txt

# Used to reduce text to ascii, exe enforce ascii in some cases.
def convertToAscii(txt):
	txt = convertToStr(txt)
	# convert to ascii equivalent where possible.
	accent = [
		('à', 'a'), ('ä', 'a'), ('â', 'a'),
		('é', 'e'), ('è', 'e'), ('ê', 'e'),
		('ù', 'u'), ('û', 'u'), ('ü', 'u'),
		('ô', 'o'), ('õ', 'o'), ('ö', 'o'),
		('ç', 'c'), ('î', 'i'), ('ï', 'i'),
		('ß', 'ss')
	]
	while accent:
		a, b = accent.pop()
		txt = txt.replace(a, b)
	# get rid of any "above ascii ordinals" that may be left here.
	i = 0
	length = len(txt)
	while i < length:
		ordinal = ord(txt[i])
		if ordinal > 128:
			txt = txt[:i] + '?' + txt[i+1:]
		i += 1
	return txt


###########
# Utility #
###########

def floatToString(fFloat, iMaxDecimal=2):
	if not fFloat: return ""
	i = iMaxDecimal
	szRaw = str(fFloat)
	index = szRaw.find(".")
	if index == -1:
		return szRaw
	if index + i > len(szRaw)-1:
		i = len(szRaw) - 1 - index
	while True:
		if not i:
			return szRaw[:index]
		elif szRaw[index + i] != "0":
			iRange = index + i + 1
			break
		i -= 1
	szString = ""
	for index in xrange(iRange):
		szString += szRaw[index]
	return szString

def evalTextWidth(iMax, uFont, szTxt):
	#print "evalTextWidth: " + szTxt
	iWidth = CyIF.determineWidth(uFont + szTxt)
	#print ("iMax", iMax)
	#print ("iWidth", iWidth)
	if iWidth > iMax:
		iChange = iCrop = len(szTxt)/2
		iChange = iCrop/2
		iMaxCrop = 0
		bLast = False
		while True:
			#print ("iCrop", iCrop)
			#print szTxt[:iCrop]
			iWidth = CyIF.determineWidth(uFont + szTxt[:iCrop])
			#print ("iWidth", iWidth)
			#print ("iChange", iChange)
			if iWidth > iMax:
				iCrop -= iChange
			else:
				if iCrop > iMaxCrop:
					iMaxCrop = iCrop
				iCrop += iChange
			if iChange:
				if iChange == 3:
					iChange = 2
				elif iChange == 1:
					if bLast:
						iChange = 0
					bLast = True # I'm not sure if the extra step is necessary...
				else:
					iChange /= 2
			else:
				return uFont + szTxt[:iMaxCrop] + "."
	return uFont + szTxt