from CvPythonExtensions import *
import SystemPaths as SP
import os
import os.path
import CvUtil

RemoveTag = (3, "BuildingClass")


def verifyModDir(szModPath):
    git_directory = os.path.join(szModPath, "git_directory.txt")
    if os.path.isfile(git_directory):
        pFile = open(git_directory)
        szModPath = pFile.read().rstrip("\n")
        pFile.close()
    return szModPath


def init():
	folder = os.path.join(verifyModDir(SP.modDir), "Assets", "Python", "Matt", "xml")
	if RemoveTag:
		for file in os.listdir(folder):
			removeTags(os.path.join(folder, file))


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

		if info.getMaxGlobalInstances() > 0:
			maxGlobal.append(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType())

		if info.getMaxTeamInstances() > 0:
			maxTeam.append(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType())

		if info.getMaxPlayerInstances() > 0:
			maxPlayer.append(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType())

		if info.getExtraPlayerInstances() > 0:
			extraPlayer.append(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType())

		if info.isNoLimit() > 0:
			noLimit.append(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType())

		if info.isMonument() > 0:
			monument.append(GC.getBuildingInfo(info.getDefaultBuildingIndex()).getType())

	return (maxGlobal, maxTeam, maxPlayer, extraPlayer, noLimit, monument)


def readFile(path):
	global location
	oldFile = open(path)
	fileInfo = []
	bComment = False
	for line in oldFile:
		location = 0
		if bComment:
			bComment = not findCommentEnd(line, location)
		else:
			numTabs = countWhiteSpace(line)
			location = numTabs +1
			if line[location] == "!":
				bComment = not findCommentEnd(line, location)
			else:
				tagName = getTagName(line)
				if location < len(line)
	fileInfo.append(LineInfo(numTabs, tagName))
	oldFile.close()


class LineInfo:

	def __init__(self, numTabs, tagName = None):
		self.numTabs = numTabs
		self.tagName = tagName


def removeTags(path):
	oldFile = open(path)
	newLines = ""
	log = ""
	for line in oldFile:
		log += "Tabs: %s Tag: %s\n" %(numTabs, tagName)
		if not (numTabs == RemoveTag[0] and tagName == RemoveTag[1]):
			newLines += line
	oldFile.close()

	newFile = open(path + ".new", "w")
	newFile.write(newLines)
	newFile.close()


def countWhiteSpace(line):
	i = 0
	while line[i] == "	" or line[i] == " ":
		i += 1
	return i


def getTagName(line, location):
	global location
	tagName = ""
	for i in range(tagStart, len(line)):
		location += 1
		if line[i] == ">":
			return tagName
		tagName += line[i]


def getElement(line, elementStart):
	element = ""
	i = elementStart
	while line[i] != "<":
		element += line[i]
	return element


def findCommentEnd(line):
	for i in range(location, len(line)):
		if line[i] == ">":
			return True
	return False
