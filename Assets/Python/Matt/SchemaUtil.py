from CvPythonExtensions import *
from XmlUtil import *
import os.path


def init():
	logFile = open(os.path.join(g_ModDir, "IntTags.h"), "w")
	folder = os.path.join(g_ModDir, "Assets", "XML", "Schema")
	for path in getGlob(folder):
		for tag in getIntTags(path):
			logFile.write("%s\n" %tag[0])
	logFile.write("\n")
	logFile.close()


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
				intTags.append((tracker.getNextSplice("\""), 0, 0))
	file.close()
	return intTags
