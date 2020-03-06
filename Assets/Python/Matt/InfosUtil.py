from CvPythonExtensions import *
import SystemPaths as SP
import os.path
import glob

RemoveTag = (3, "BuildingClass")


def verifyModDir(szModPath):
    git_directory = os.path.join(szModPath, "git_directory.txt")
    if os.path.isfile(git_directory):
        pFile = open(git_directory)
        szModPath = pFile.read().rstrip("\n")
        pFile.close()
    return szModPath


def init():
	paths  = glob.glob(os.path.join(verifyModDir(SP.modDir), "Assets", "Python", "Matt", "xml", "*.xml"))
	paths += glob.glob(os.path.join(verifyModDir(SP.modDir), "Assets", "Python", "Matt", "xml", "**", "*.xml"))
	paths += glob.glob(os.path.join(verifyModDir(SP.modDir), "Assets", "Python", "Matt", "xml", "**", "**", "*.xml"))
	paths += glob.glob(os.path.join(verifyModDir(SP.modDir), "Assets", "Python", "Matt", "xml", "**", "**", "**", "*.xml"))
	for path in paths:
		if os.path.isfile(path):
			if RemoveTag:
				writeFile(removeTags(readFile(path)), open(path, "w"))
			writeFile(moveTags(readFile(path), buildCache()), open(path, "w"))


def writeFile(lines, file):
	file.write(lines)
	file.close()


def buildCache():
	GC = CyGlobalContext

	maxGlobal = []
	maxTeam = []
	maxPlayer = []
	extraPlayer = []
	noLimit = []
	monument = []

	for i in range(GC.getNumBuildingClassInfos()):
		info = GC.getBuildingClassInfo(i)

		j = info.getMaxGlobalInstances()
		if j > 0:
			line = "			<iMaxGlobalInstances>%d</iMaxGlobalInstances>\n" %j
			maxGlobal.append(MoveTag(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType(), j))

		j = info.getMaxTeamInstances()
		if j > 0:
			line = "			<iMaxTeamInstances>%d</iMaxTeamInstances>\n" %j
			maxTeam.append(MoveTag(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType(), j))

		j = info.getMaxPlayerInstances()
		if j > 0:
			line = "			<iMaxPlayerInstances>%d</iMaxPlayerInstances>\n" %j
			maxPlayer.append(MoveTag(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType(), j))

		j = info.getExtraPlayerInstances()
		if j > 0:
			line = "			<iExtraPlayerInstances>%d</iExtraPlayerInstances>\n" %j
			extraPlayer.append(MoveTag(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType(), j))

		j = info.isNoLimit()
		if j > 0:
			line = "			<bNoLimit>%d</bNoLimit>\n" %j
			noLimit.append(MoveTag(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType(), j))

		j = info.isMonument()
		if j > 0:
			line = "			<bMonument>%d</bMonument>\n" %j
			monument.append(MoveTag(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType(), j))

	return (maxGlobal, maxTeam, maxPlayer, extraPlayer, noLimit, monument)


class MoveTag:

	def __init__(self, typeName, num, line):
		self.typeName = typeName
		self.num = num
		self.line = line
		self.insert = False

	def matchType(self, typeName):
		if self.typeName == typeName:
			self.insert = True


def moveTags(lineInfo, argsList):
	insert = None
	newLines = ""
	for i in range(len(lineInfo)):
		newLines += lineInfo[i].line

		if lineInfo[i].tagName == "type":
			argsList = matchTypes(lineInfo[i].element, argsList)

		if insert == None:
			insert = findTypeEnd(i, lineInfo)

		elif i == insert:
			for tagType in range(len(argsList)):
				for tag in range(len(argsList[tagType])):
					if argsList[tagType][tag].insert:
						newLine += argsList[tagType][tag].line
						argsList[tagType][tag].insert = False
			insert = None


def matchTypes(typeName, argsList):
	for tagType in range(len(argsList)):
		for tag in range(len(argsList[tagType])):
			argsList[tagType][tag].matchType(typeName)
	return argsList
				

def findTypeEnd(startingLine, fileInfo):
	for line in range(startingLine, len(fileInfo)):
		if fileInfo[line].numTabs == 3:
			return line


def readFile(path):
	global location
	oldFile = open(path)
	fileInfo = []
	bComment = False
	for line in oldFile:
		location = 0
		numTabs = 0
		tagName = None
		element = None
		if not bComment:
			numTabs = countWhiteSpace(line)
			location = numTabs +1
			if line[location] == "!":
				bComment = True
			else:
				tagName = getTagName(line)
				location += 1
				if location < len(line):
					element = getElement(line)
		fileInfo.append(LineInfo(line, numTabs, tagName, element))
		if bComment:
			bComment = not findCommentEnd(line)
	oldFile.close()
	return fileInfo


class LineInfo:

	def __init__(self, line, numTabs, tagName, element):
		self.line = line
		self.numTabs = numTabs
		self.tagName = tagName
		self.element = element


def writeLog():
	for file in os.listdir(os.path.join(verifyModDir(SP.modDir), "Assets", "Python", "Matt", "xml")):
		for line in readFile(file):
			log += "Tabs: %s Tag: %s Element: %s Comment: %s" %(line.numTabs, line.tagName, line.element, line.bComment)


def findHighestElement(fileInfo, tagName):
	largestElement = 0
	for line in fileInfo:
		if line.tagName == tagName and line.element > largestElement:
			largestElement = line.element
	return largestElement


def removeTags(fileInfo):
	newLines = ""
	for line in oldFile:
		if not (numTabs == RemoveTag[0] and tagName == RemoveTag[1]):
			newLines += line
	return newLines


def countWhiteSpace(line):
	i = 0
	while line[i] == "	" or line[i] == " ":
		i += 1
	return i


def getTagName(line):
	global location
	tagName = ""
	for i in range(tagStart, len(line)):
		location += 1
		if line[i] == ">":
			return tagName
		tagName += line[i]


def getElement(line):
	element = ""
	while line[location] != "<":
		location += 1
		element += line[location]
	return element


def findCommentEnd(line):
	for i in range(location, len(line)):
		if line[i] == ">":
			return True
	return False

	return ">" in line[location:len(line)]

