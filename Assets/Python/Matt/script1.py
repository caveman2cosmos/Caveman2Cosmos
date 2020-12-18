from CvPythonExtensions import *
from os.path import join
import SchemaUtil
import XmlUtil
import EnumerateInfoTypes


def init():
	dir = XmlUtil.g_ModDir
	xmlPaths  = XmlUtil.getGlob(join(dir, "Assets", "XML"))
	xmlPaths += XmlUtil.getGlob(join(dir, "Assets", "Modules"))
	infoClassData = XmlUtil.g_InfoClassData
	for classData in infoClassData:
		intTags = SchemaUtil.getIntTags(join(dir, "Assets", "XML", "Schema", classData[2] + ".xml"))
		for xmlPath in xmlPaths:
			numChars = len(xmlPath)
			ext = xmlPath[numChars - len(classData[1]) - 4 : numChars - 4]
			if ext == classData[1]:
				for tagData in intTags:
					xmlFile = open(xmlPath, "r")
					for line in xmlFile:
						element = EnumerateInfoTypes.getElement(line, tagData[0])
						if element is not None:
							if element < tagData[1]:
								tagData[1] = element
							if element > tagData[2]:
								tagData[2] = element
					xmlFile.close()
		logIntTagData(classData, intTags)


def logIntTagData(classData, intTags):
	logFile = open(join(XmlUtil.g_ModDir, "IntTagData.log"), "a")
	logFile.write(classData[1] + "\n")
	for tagData in intTags:
		logFile.write("%s min: %s max: %s\n" %(tagData[0], tagData[1], tagData[2]))
	logFile.write("\n\n")
	logFile.close()