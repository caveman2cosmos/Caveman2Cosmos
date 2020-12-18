from CvPythonExtensions import *
from XmlUtil import *


def getIntTags(path):
	intTags = []
	file = open(path)
	for line in file:
		tracker = LineReader(line)
		tracker.skipWhiteSpace()
		if tracker.matchNextChar("<") and tracker.getNextSplice("=") == "ElementType name":
			numChars = len(line) - 3
			dataType = line[numChars - 13 : numChars]
			#-----------------------------------
			if DEBUG_LOG: tracker.log(dataType)
			#-----------------------------------
			if dataType == "dt:type=\"int\"":
				tracker.skipNextChar()
				tracker.skipNextChar()
				intTags.append([tracker.getNextSplice("\""), 0, 0])
				#yield [tracker.getNextSplice("\""), 0, 0]
	file.close()
	return intTags