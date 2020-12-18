from CvPythonExtensions import *
from os.path import join
import XmlUtil


def init():
	infoClassData = XmlUtil.g_InfoClassData
	dir = XmlUtil.g_ModDir
	hFile = open(join(dir, "Sources", "InfoTypes.h"), "w")
	xmlPaths  = XmlUtil.getGlob(join(dir, "Assets", "XML"))
	xmlPaths += XmlUtil.getGlob(join(dir, "Assets", "Modules"))
	for classData in infoClassData:
		infoTypes = []
		for xmlPath in xmlPaths:
			numChars = len(xmlPath)
			ext = xmlPath[numChars - len(classData[1]) - 4 : numChars - 4]
			if ext == classData[1]:
				readInfoTypes(open(xmlPath, "r"), infoTypes)
		writeInfoTypes(hFile, classData, infoTypes)
	hFile.close()


def writeInfoTypes(hFile, infoClassData, infoTypes):
	numTypes = len(infoTypes)
	if numTypes > 0:
		for i in range(numTypes):
			hFile.write("#define %s %d\n" %(infoTypes[i], i))
		hFile.write("\n#define NUM_%s %d\n\n" %(infoClassData[0], i +1))


def readInfoTypes(xmlFile, infoTypes):
	for line in xmlFile:
		element = getElement(line, "Type")
		if element is not None and not element in infoTypes:
			infoTypes.append(element)
	xmlFile.close()


def getElement(line, tag):
	if line[len(line) -2] == ">":
		tagStart = countWhiteSpace(line)
		if line[tagStart] == "<" and spliceLine(line, tagStart +1, ">") == tag:
			return spliceLine(line, tagStart + len(tag) + 2, "<")
	return None


def countWhiteSpace(line, location = 0):
	while line[location] == "	" or line[location] == " ":
		location += 1
	return location


def spliceLine(line, location, endChar):
	data = ""
	while line[location] != endChar:
		data += line[location]
		location += 1
	return data
