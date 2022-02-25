#
# TestCode - tab within DebugScreen
#
# Toffer - I intentionally made this very bare bones, hiding technical stuff inside DebugScreen.py
#	This so that python novices can utilize this file as a training/testing ground with minimal intimidation.
from CvPythonExtensions import *
GC = CyGlobalContext()

class TestCode:

	def __init__(self, pDebugScreen):
		self.main = pDebugScreen

		import HelperFunctions
		self.HF = HelperFunctions.HelperFunctions([0])

	# Use this instead of print, it will print to pythonDbg and will also print on screen.
	def log(self, text):
		self.main.testCodelog(text)

	def drawTab(self, screen):
		# Toffer - Add new test code button to the debug screen like this and define the function below.
		#	This is all that is needed for the button to become functional, one line.
		#self.main.addTestCode(screen, functionName, "Button Text", "tooltip")
		self.main.addTestCode(screen, self.checkBuildingRequirements, "Building requirements of buildings", "Checks if building requirements aren't unlocked after building itself")
		self.main.addTestCode(screen, self.checkBuildingRequirementReplacements, "Building requirement replacements", "Checks if building requirements are replaced")
		self.main.addTestCode(screen, self.checkBuildingRequirementObsoletion, "Building obsoletion of requirements", "Checks when requirements obsolete in relation to building itself. Building requirements of building shouldn't obsolete before building itself. For beeliners: If there is requirement obsoletion within 10 columns, then building shall obsolete on same tech as its requirement")
		self.main.addTestCode(screen, self.checkBuildingUnlockObsoletion, "Buildings unlock/obsoletion", "Checks if building obsoletion doesn't happen within 5 columns of building unlock")
		self.main.addTestCode(screen, self.checkBuildingReplacementObsoletion, "Building obsoletion of replacements", "Checks when replacements are unlocked and obsoleted. Replacing building shouldn't unlock or obsolete before replaced one")
		self.main.addTestCode(screen, self.checkBuildingImplicitReplacements, "Building - check implicit replacements", "Check if we have implicit replacements - All replacements must be explicitly defined even if building got obsoleted long ago")
		self.main.addTestCode(screen, self.checkBuildingReplacingQuality, "Building - check replacement quality", "Check if building, that replaces earlier buildings is better in various metrics. Yields - Food, Production, Commerce. Commerces - Gold, Research, Culture, Espionage. Building productivity is shown as: currently checked building / sum of replaced building tag values")
		self.main.addTestCode(screen, self.checkBuildingReplacingAvailability, "Building - check replacement availability", "Check if replaced buildings are affected by other buildings, civics, traits, conversely improvements can upgrade, so they are checked too")
		self.main.addTestCode(screen, self.checkBuildingFreeReward, "Building unlock/obsoletion of free buildings", "Checks if free buildings - normally unbuildable - obsolete or unlock together with building, that gives it for free.")
		self.main.addTestCode(screen, self.checkBuildingBonusRequirements, "Building bonus requirements", "Checks various bonus prereqs to check if they aren't unlocked after building")
		self.main.addTestCode(screen, self.checkBuildingBonusManufacturerTech, "Building earliest manufacturer on resource tech reveal", "Checks when earliest resource producer is unlocked")
		self.main.addTestCode(screen, self.checkBuildingRequirementTags, "Building - requirement requirements", "Check if additonal requirements don't lock out buildings")
		self.main.addTestCode(screen, self.checkBuildingRequirementCivics, "Building - requirement civic requirements", "Check if building requirements require civics")
		self.main.addTestCode(screen, self.checkBuildingCivicRequirements, "Building - civic requirements and obsoletions", "Checks if various civics aren't unlocked after building. Civic buildings shouldn't obsolete, as effectively it represents unique conditions under that civic")
		self.main.addTestCode(screen, self.checkBuildingCivicInfluences, "Building - check civic tags", "Check if building is available when civic is active")
		self.main.addTestCode(screen, self.checkBuildingTechMods, "Building tech changes and modifiers", "Checks if tech modifiers and changes occur within building lifetime")
		self.main.addTestCode(screen, self.checkBuildingBonusTags, "Building - check bonus tags", "Check if bonus tech reveal is after building obsoletion - those bonus tags affect buildings in various ways")
		self.main.addTestCode(screen, self.checkBuildingAffectingBuildings, "Building - check building tags", "Check if building affecting other building is within lifetime of each other")
		self.main.addTestCode(screen, self.checkBuildingReligionRequirement, "Building - check consistency of religion tags", "Checks if tags requiring religion share same religion")
		self.main.addTestCode(screen, self.checkBuildingTags, "Building Tags", "Checks if commerce double time exists on wonders, that have relevant flat commerce change, if Commerce Change has relevant flat commerce changes, if hurry modifiers exist on unbuildable buildings, if GP unit references are paired with GP changes, or if freebonus amount is paired with bonus")
		self.main.addTestCode(screen, self.checkBuildingMinYields, "Building - check yields", "Check if buildings with yield income have at least minimum yield as derived from era")
		self.main.addTestCode(screen, self.checkBuildingTechChanges, "Building - check Tech Changes", "Check if buildings with tech yield/commerce changes have minimum gains from techs")
		self.main.addTestCode(screen, self.checkBuildingMaxMaint, "Building - check maintenance", "Check if buildings don't take too much gold for its output")
		self.main.addTestCode(screen, self.checkTechCosts, "Tech - check costs", "Check if techs have correct costs")
		self.main.addTestCode(screen, self.checkBuildingCosts, "Building - check costs", "Check if buildings have correct costs")
		self.main.addTestCode(screen, self.checkUnitCosts, "Unit - check costs", "Check if unit costs are within sane limits")
		self.main.addTestCode(screen, self.checkUnitUpgrades, "Unit - check unit upgrades", "Checks unit upgrades")
		self.main.addTestCode(screen, self.checkUnitBonusRequirements, "Unit - check bonus requirements", "Checks bonus requirements of units")
		self.main.addTestCode(screen, self.checkUnitRequirements, "Unit - check building requirements", "Checks building requirements of units")
		self.main.addTestCode(screen, self.checkUnitRequirementsReplacements, "Unit - check building requirement replacements", "Checks if unit has building requirement, that gets replaced")
		self.main.addTestCode(screen, self.checkUnitAis, "Unit - Check default and general unit AIs", "Checks if unit default AI is listed in unit AIs")
		self.main.addTestCode(screen, self.checkBonusImprovementProductivity, "Bonus - check improvement productivity", "Checks if improvement replacements productivity from bonus, improvement and bonus+improvement is higher compared to base improvement")
		self.main.addTestCode(screen, self.checkBonusProducerReplacements, "Bonus - check potential bonus producer replacements", "Checks replacements of manufactured bonus producers")
		self.main.addTestCode(screen, self.checkCivicImprovementReplacements, "Civic - check potential improvement replacements", "Checks replacements of improvements in civics")
		self.main.addTestCode(screen, self.checkTraitImprovementReplacements, "Trait - check potential improvement replacements", "Checks replacements of improvements in traits")
		self.main.addTestCode(screen, self.checkUnitBuildTechAligment, "Unit and Build - check earliest worker builds", "Checks if earliest worker is unlocked when build is available")
		self.main.addTestCode(screen, self.checkBuildImprovementTechAligment, "Improvement and Build - check if they are unlocked on same tech", "Checks if improvement and build have same tech unlock")
		self.main.addTestCode(screen, self.checkImprovementResourceTechUnlocks, "Improvement - check its unlock tech along with its resource tech enable", "Checks if earliest valid improvement isn't unlocked after resource tech enable")
		self.main.addTestCode(screen, self.checkImprovementTechYieldBoostLocation, "Improvement - yield boost tech requirements", "Checks if yield boosts happen within tech unlock and replacement of improvements")
		self.main.addTestCode(screen, self.checkImprovementYieldValues, "Improvement - all techs boosts compared to upgrade", "Checks if improvement with all tech boosts isn't better than its upgrade")
		self.main.addTestCode(screen, self.checkImprovementBonusSpawnChance, "Improvement - check bonus chance", "Checks if upgrade of improvement is more likely to spawn resource")
		self.main.addTestCode(screen, self.checkBuildingWonderMovies, "Building movie wonder list", "Checks movies of noncultural wonders, religious shrines and projects movie location")
		self.main.addTestCode(screen, self.checkTechTypes, "Building and unit - Tech Types check", "Checks if buildings and units main tech is more advanced or equal to Tech Type")
		self.main.addTestCode(screen, self.listStandaloneBuildings, "Building - list stand-alone buildings", "List regular non religious/civic buildings, that aren't part of replacement chain")
		self.main.addTestCode(screen, self.countUnlockedObsoletedBuildings, "Building - list unlocks/obsoletions", "List how many buildings got unlocked/obsoleted")
		self.main.addTestCode(screen, self.countBonusProducers, "Building - list amount of bonus providers", "List how many buildings provide bonuses")
		self.main.addTestCode(screen, self.checkTaxonomyBuildings, "Building - list potential Taxonomy requirements", "List taxonomy buildings, that doesn't have all potential base folklore requirements")

	#Building requirements of buildings
	def checkBuildingRequirements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iBaseTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]
			aBaseTechIDList = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[2]
			aBaseTechList = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[3]

			#<PrereqInCityBuildings> - require all buildings in list
			aRequirementTechLocList = []
			aReqColumnTechIDList = []
			aReqColumnTechList = []
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
				aRequirementTechLocList.append(self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
						aReqColumnTechList.append(aRequirementTechList[i])
			if len(aRequirementTechLocList) > 0 and max(aRequirementTechLocList) > iBaseTechLoc:
				self.log(CvBuildingInfo.getType()+" is unlocked before its AND building requirements "+str(aRequirementTechList)+" "+str(aBaseTechList)+" requirement/base column "+str(max(aRequirementTechLocList))+" / "+str(iBaseTechLoc))
			if len(aRequirementTechLocList) > 0 and max(aRequirementTechLocList) == iBaseTechLoc:
				for i in xrange(len(aReqColumnTechIDList)):
					if aReqColumnTechIDList[i] not in aBaseTechIDList:
						self.log(CvBuildingInfo.getType()+" AND requirement unlocked by different tech in column: "+str(iBaseTechLoc)+" requirement/base "+str(aReqColumnTechList[i])+" / "+str(aBaseTechList))

			#<PrereqOrBuildings> - require one building in list
			aRequirementTechLocList = []
			aReqColumnTechIDList = []
			aReqColumnTechList = []
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuildingRequirement)
				aRequirementTechLocList.append(self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
						aReqColumnTechList.append(aRequirementTechList[i])
			if len(aRequirementTechLocList) > 0 and min(aRequirementTechLocList) > iBaseTechLoc:
				self.log(CvBuildingInfo.getType()+" is unlocked before its earliest OR building requirements "+str(aRequirementTechList)+" "+str(aBaseTechList)+" requirement/base column "+str(min(aRequirementTechLocList))+" / "+str(iBaseTechLoc))
			if len(aRequirementTechLocList) > 0 and min(aRequirementTechLocList) == iBaseTechLoc:
				for i in xrange(len(aReqColumnTechIDList)):
					if aReqColumnTechIDList[i] not in aBaseTechIDList:
						self.log(CvBuildingInfo.getType()+" Earliest OR requirement unlocked by different tech in column: "+str(iBaseTechLoc)+" requirement/base "+str(aReqColumnTechList[i])+" / "+str(aBaseTechList))

			#<PrereqAmountBuildings> - require all buildings in empire in list
			aRequirementTechLocList = []
			aReqColumnTechIDList = []
			aReqColumnTechList = []
			for iPrereqBuilding, iNumRequired in CvBuildingInfo.getPrereqNumOfBuildings():
				aRequirementTechLocList.append(self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
						aReqColumnTechList.append(aRequirementTechList[i])
			if len(aRequirementTechLocList) > 0 and max(aRequirementTechLocList) > iBaseTechLoc:
				self.log(CvBuildingInfo.getType()+" is unlocked before its empire AND building requirements "+str(aRequirementTechList)+" "+str(aBaseTechList)+" requirement/base column "+str(max(aRequirementTechLocList))+" / "+str(iBaseTechLoc))
			if len(aRequirementTechLocList) > 0 and max(aRequirementTechLocList) == iBaseTechLoc:
				for i in xrange(len(aReqColumnTechIDList)):
					if aReqColumnTechIDList[i] not in aBaseTechIDList:
						self.log(CvBuildingInfo.getType()+" Empire AND requirement unlocked by different tech in column: "+str(iBaseTechLoc)+" requirement/base "+str(aReqColumnTechList[i])+" / "+str(aBaseTechList))

			#<ConstructCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.HF.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aRequirementTechLocList = []
			aReqColumnTechIDList = []
			aReqColumnTechList = []
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuildingRequirement]
				aRequirementTechLocList.append(self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
						aReqColumnTechList.append(aRequirementTechList[i])
			if len(aRequirementTechLocList) > 0 and max(aRequirementTechLocList) > iBaseTechLoc:
				self.log(CvBuildingInfo.getType()+" is unlocked before its GOM AND building requirements "+str(aRequirementTechList)+" "+str(aBaseTechList)+" requirement/base column "+str(max(aRequirementTechLocList))+" / "+str(iBaseTechLoc))
			if len(aRequirementTechLocList) > 0 and max(aRequirementTechLocList) == iBaseTechLoc:
				for i in xrange(len(aReqColumnTechIDList)):
					if aReqColumnTechIDList[i] not in aBaseTechIDList:
						self.log(CvBuildingInfo.getType()+" GOM AND requirement unlocked by different tech in column: "+str(iBaseTechLoc)+" requirement/base "+str(aReqColumnTechList[i])+" / "+str(aBaseTechList))

			#Analyze GOM OR Building reqs
			aRequirementTechLocList = []
			aReqColumnTechIDList = []
			aReqColumnTechList = []
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuildingRequirement]
				aRequirementTechLocList.append(self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
						aReqColumnTechList.append(aRequirementTechList[i])
			if len(aRequirementTechLocList) > 0 and min(aRequirementTechLocList) > iBaseTechLoc:
				self.log(CvBuildingInfo.getType()+" is unlocked before its GOM OR building requirements "+str(aRequirementTechList)+" "+str(aBaseTechList)+" requirement/base column "+str(min(aRequirementTechLocList))+" / "+str(iBaseTechLoc))
			if len(aRequirementTechLocList) > 0 and min(aRequirementTechLocList) == iBaseTechLoc:
				for i in xrange(len(aReqColumnTechIDList)):
					if aReqColumnTechIDList[i] not in aBaseTechIDList:
						self.log(CvBuildingInfo.getType()+" GOM OR requirement unlocked by different tech in column: "+str(iBaseTechLoc)+" requirement/base "+str(aReqColumnTechList[i])+" / "+str(aBaseTechList))

			#<PrereqNotInCityBuildings> - Those block building from being built, they should be unlocked at same tech if they are regular building
			for i in xrange(CvBuildingInfo.getNumPrereqNotInCityBuildings()):
				iBlockerBuilding = CvBuildingInfo.getPrereqNotInCityBuilding(i)
				CvBlockerBuilding = GC.getBuildingInfo(iBlockerBuilding)
				if max(aBaseTechIDList) != max(self.HF.checkBuildingTechRequirements(CvBlockerBuilding)[2]) and not isNationalWonder(iBlockerBuilding) and not isWorldWonder(iBlockerBuilding) and CvBlockerBuilding.getProductionCost() > 0:
					self.log(CvBuildingInfo.getType()+" doesn't unlock with "+CvBlockerBuilding.getType())

	#Building replacements of requirements
	def checkBuildingRequirementReplacements(self):
		aSpecialReplacementsList = ["BUILDING_POLLUTION_BLACKENEDSKIES", "BUILDING_GAMBLING_BAN", "BUILDING_ALCOCHOL_PROHIBITION", "BUILDING_DRUG_PROHIBITION", "BUILDING_PROSTITUTION_BAN", "BUILDING_EMANCIPATION_PROCLAMATION_EFFECT"]
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)

			#Generate list of buildings, that replace currently checked building
			aBuildingReplacementList = []
			for iBuildingReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				iReplacementBuilding = CvBuildingInfo.getReplacementBuilding(iBuildingReplacement)
				if iReplacementBuilding not in aBuildingReplacementList:
					aBuildingReplacementList.append(iReplacementBuilding)
			#Generate list of buildings, that are replaced by currently checked building
			aBuildingReplacedList = []
			for iBuildingReplaced in xrange(CvBuildingInfo.getNumReplacedBuilding()):
				iReplacedBuilding = CvBuildingInfo.getReplacedBuilding(iBuildingReplaced)
				if iReplacedBuilding not in aBuildingReplacedList:
					aBuildingReplacedList.append(iReplacedBuilding)

			#<PrereqInCityBuildings> - require all buildings in list
			aBuildingInCityRequirementList = []
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				aBuildingInCityRequirementList.append(CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement))
			#Generate list of buildings, that replace requirements, ignore bans as their intent is to block stuff
			aBuildingRequirementReplacementList = []
			for i in xrange(len(aBuildingInCityRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingInCityRequirementList[i])
				for iBuildingReplacement in xrange(CvBuildingRequirementInfo.getNumReplacementBuilding()):
					iReplacementBuilding = CvBuildingRequirementInfo.getReplacementBuilding(iBuildingReplacement)
					if iReplacementBuilding not in aBuildingRequirementReplacementList and GC.getBuildingInfo(iReplacementBuilding).getType() not in aSpecialReplacementsList:
						aBuildingRequirementReplacementList.append(iReplacementBuilding)
			#Generate list of replaced requirements, that aren't listed as requirements
			aUnlistedRequirementReplacements = []
			for i in xrange(len(aBuildingRequirementReplacementList)):
				if aBuildingRequirementReplacementList[i] not in aBuildingInCityRequirementList:
					aUnlistedRequirementReplacements.append(aBuildingRequirementReplacementList[i])
			#Check if we have requirement, that is replacement of building
			for i in xrange(len(aBuildingInCityRequirementList)):
				if aBuildingInCityRequirementList[i] in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" AND is replaced by requirement: "+GC.getBuildingInfo(aBuildingInCityRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by building itself
			for i in xrange(len(aBuildingInCityRequirementList)):
				if aBuildingInCityRequirementList[i] in aBuildingReplacedList:
					self.log(CvBuildingInfo.getType()+" AND replaces requirement: "+GC.getBuildingInfo(aBuildingInCityRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by unlisted requirement replacement, ignore requirement replacement, if it replaces currently checked building
			for i in xrange(len(aUnlistedRequirementReplacements)):
				if aUnlistedRequirementReplacements[i] not in aBuildingInCityRequirementList and aUnlistedRequirementReplacements[i] not in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" AND has unlisted replaced requirement: "+GC.getBuildingInfo(aUnlistedRequirementReplacements[i]).getType())

			#<PrereqOrBuildings> - require one building in list
			aBuildingOrRequirementList = []
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				aBuildingOrRequirementList.append(CvBuildingInfo.getPrereqOrBuilding(iBuildingRequirement))
			#Generate list of buildings, that replace requirements, ignore bans as their intent is to block stuff
			aBuildingRequirementReplacementList = []
			for i in xrange(len(aBuildingOrRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingOrRequirementList[i])
				for iBuildingReplacement in xrange(CvBuildingRequirementInfo.getNumReplacementBuilding()):
					iReplacementBuilding = CvBuildingRequirementInfo.getReplacementBuilding(iBuildingReplacement)
					if iReplacementBuilding not in aBuildingRequirementReplacementList and GC.getBuildingInfo(iReplacementBuilding).getType() not in aSpecialReplacementsList:
						aBuildingRequirementReplacementList.append(iReplacementBuilding)
			#Generate list of replaced requirements, that aren't listed as requirements
			aUnlistedRequirementReplacements = []
			for i in xrange(len(aBuildingRequirementReplacementList)):
				if aBuildingRequirementReplacementList[i] not in aBuildingOrRequirementList:
					aUnlistedRequirementReplacements.append(aBuildingRequirementReplacementList[i])
			#Check if we have requirement, that is replacement of building
			for i in xrange(len(aBuildingOrRequirementList)):
				if aBuildingOrRequirementList[i] in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" OR is replaced by requirement: "+GC.getBuildingInfo(aBuildingOrRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by building itself
			for i in xrange(len(aBuildingOrRequirementList)):
				if aBuildingOrRequirementList[i] in aBuildingReplacedList:
					self.log(CvBuildingInfo.getType()+" OR replaces requirement: "+GC.getBuildingInfo(aBuildingOrRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by unlisted requirement replacement, ignore requirement replacement, if it replaces currently checked building
			for i in xrange(len(aUnlistedRequirementReplacements)):
				if aUnlistedRequirementReplacements[i] not in aBuildingOrRequirementList and aUnlistedRequirementReplacements[i] not in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" OR has unlisted replaced requirement: "+GC.getBuildingInfo(aUnlistedRequirementReplacements[i]).getType())

			#<PrereqAmountBuildings> - require all buildings in empire in list
			aBuildingEmpireAndRequirementList = []
			for iPrereqBuilding, iNumRequired in CvBuildingInfo.getPrereqNumOfBuildings():
				aBuildingEmpireAndRequirementList.append(iPrereqBuilding)
			#Generate list of buildings, that replace requirements, ignore bans as their intent is to block stuff
			aBuildingRequirementReplacementList = []
			for i in xrange(len(aBuildingEmpireAndRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingEmpireAndRequirementList[i])
				for iBuildingReplacement in xrange(CvBuildingRequirementInfo.getNumReplacementBuilding()):
					iReplacementBuilding = CvBuildingRequirementInfo.getReplacementBuilding(iBuildingReplacement)
					if iReplacementBuilding not in aBuildingRequirementReplacementList and GC.getBuildingInfo(iReplacementBuilding).getType() not in aSpecialReplacementsList:
						aBuildingRequirementReplacementList.append(iReplacementBuilding)
			#Generate list of replaced requirements, that aren't listed as requirements
			aUnlistedRequirementReplacements = []
			for i in xrange(len(aBuildingRequirementReplacementList)):
				if aBuildingRequirementReplacementList[i] not in aBuildingEmpireAndRequirementList:
					aUnlistedRequirementReplacements.append(aBuildingRequirementReplacementList[i])
			#Check if we have requirement, that is replacement of building
			for i in xrange(len(aBuildingEmpireAndRequirementList)):
				if aBuildingEmpireAndRequirementList[i] in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" Empire AND is replaced by requirement: "+GC.getBuildingInfo(aBuildingEmpireAndRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by building itself
			for i in xrange(len(aBuildingEmpireAndRequirementList)):
				if aBuildingEmpireAndRequirementList[i] in aBuildingReplacedList:
					self.log(CvBuildingInfo.getType()+" Empire AND replaces requirement: "+GC.getBuildingInfo(aBuildingEmpireAndRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by unlisted requirement replacement, ignore requirement replacement, if it replaces currently checked building
			for i in xrange(len(aUnlistedRequirementReplacements)):
				if aUnlistedRequirementReplacements[i] not in aBuildingEmpireAndRequirementList and aUnlistedRequirementReplacements[i] not in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" Empire AND has unlisted replaced requirement: "+GC.getBuildingInfo(aUnlistedRequirementReplacements[i]).getType())

			#<ConstructCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.HF.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aBuildingGOMAndRequirementList = []
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aBuildingGOMAndRequirementList.append(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuildingRequirement])
			#Generate list of buildings, that replace requirements, ignore bans as their intent is to block stuff
			aBuildingRequirementReplacementList = []
			for i in xrange(len(aBuildingGOMAndRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingGOMAndRequirementList[i])
				for iBuildingReplacement in xrange(CvBuildingRequirementInfo.getNumReplacementBuilding()):
					iReplacementBuilding = CvBuildingRequirementInfo.getReplacementBuilding(iBuildingReplacement)
					if iReplacementBuilding not in aBuildingRequirementReplacementList and GC.getBuildingInfo(iReplacementBuilding).getType() not in aSpecialReplacementsList:
						aBuildingRequirementReplacementList.append(iReplacementBuilding)
			#Generate list of replaced requirements, that aren't listed as requirements
			aUnlistedRequirementReplacements = []
			for i in xrange(len(aBuildingRequirementReplacementList)):
				if aBuildingRequirementReplacementList[i] not in aBuildingGOMAndRequirementList:
					aUnlistedRequirementReplacements.append(aBuildingRequirementReplacementList[i])
			#Check if we have requirement, that is replacement of building
			for i in xrange(len(aBuildingGOMAndRequirementList)):
				if aBuildingGOMAndRequirementList[i] in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" GOM AND is replaced by requirement: "+GC.getBuildingInfo(aBuildingGOMAndRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by building itself
			for i in xrange(len(aBuildingGOMAndRequirementList)):
				if aBuildingGOMAndRequirementList[i] in aBuildingReplacedList:
					self.log(CvBuildingInfo.getType()+" GOM AND replaces requirement: "+GC.getBuildingInfo(aBuildingGOMAndRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by unlisted requirement replacement, ignore requirement replacement, if it replaces currently checked building
			for i in xrange(len(aUnlistedRequirementReplacements)):
				if aUnlistedRequirementReplacements[i] not in aBuildingGOMAndRequirementList and aUnlistedRequirementReplacements[i] not in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" GOM AND has unlisted replaced requirement: "+GC.getBuildingInfo(aUnlistedRequirementReplacements[i]).getType())

			#Analyze GOM OR Building reqs
			aBuildingGOMOrRequirementList = []
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aBuildingGOMOrRequirementList.append(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuildingRequirement])
			#Generate list of buildings, that replace requirements, ignore bans as their intent is to block stuff
			aBuildingRequirementReplacementList = []
			for i in xrange(len(aBuildingGOMOrRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingGOMOrRequirementList[i])
				for iBuildingReplacement in xrange(CvBuildingRequirementInfo.getNumReplacementBuilding()):
					iReplacementBuilding = CvBuildingRequirementInfo.getReplacementBuilding(iBuildingReplacement)
					if iReplacementBuilding not in aBuildingRequirementReplacementList and GC.getBuildingInfo(iReplacementBuilding).getType() not in aSpecialReplacementsList:
						aBuildingRequirementReplacementList.append(iReplacementBuilding)
			#Generate list of replaced requirements, that aren't listed as requirements
			aUnlistedRequirementReplacements = []
			for i in xrange(len(aBuildingRequirementReplacementList)):
				if aBuildingRequirementReplacementList[i] not in aBuildingGOMOrRequirementList:
					aUnlistedRequirementReplacements.append(aBuildingRequirementReplacementList[i])
			#Check if we have requirement, that is replacement of building
			for i in xrange(len(aBuildingGOMOrRequirementList)):
				if aBuildingGOMOrRequirementList[i] in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" GOM OR is replaced by requirement: "+GC.getBuildingInfo(aBuildingGOMOrRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by building itself
			for i in xrange(len(aBuildingGOMOrRequirementList)):
				if aBuildingGOMOrRequirementList[i] in aBuildingReplacedList:
					self.log(CvBuildingInfo.getType()+" GOM OR replaces requirement: "+GC.getBuildingInfo(aBuildingGOMOrRequirementList[i]).getType())
			#Check if we have requirement, that is replaced by unlisted requirement replacement, ignore requirement replacement, if it replaces currently checked building
			for i in xrange(len(aUnlistedRequirementReplacements)):
				if aUnlistedRequirementReplacements[i] not in aBuildingGOMOrRequirementList and aUnlistedRequirementReplacements[i] not in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" GOM OR has unlisted replaced requirement: "+GC.getBuildingInfo(aUnlistedRequirementReplacements[i]).getType())

			#Analyze Not In City Building blockers
			aBuildingNotInCityRequirementList = []
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqNotInCityBuildings()):
				aBuildingNotInCityRequirementList.append(CvBuildingInfo.getPrereqNotInCityBuilding(iBuildingRequirement))
			#Generate list of buildings, that replace blockers, ignore bans as their intent is to block stuff
			aBuildingRequirementReplacementList = []
			for i in xrange(len(aBuildingNotInCityRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingNotInCityRequirementList[i])
				for iBuildingReplacement in xrange(CvBuildingRequirementInfo.getNumReplacementBuilding()):
					iReplacementBuilding = CvBuildingRequirementInfo.getReplacementBuilding(iBuildingReplacement)
					if iReplacementBuilding not in aBuildingRequirementReplacementList and GC.getBuildingInfo(iReplacementBuilding).getType() not in aSpecialReplacementsList:
						aBuildingRequirementReplacementList.append(iReplacementBuilding)
			#Generate list of replaced blockers, that aren't listed as blockers
			aUnlistedRequirementReplacements = []
			for i in xrange(len(aBuildingRequirementReplacementList)):
				if aBuildingRequirementReplacementList[i] not in aBuildingNotInCityRequirementList:
					aUnlistedRequirementReplacements.append(aBuildingRequirementReplacementList[i])
			#Check if we have blockers, that is replacement of building
			for i in xrange(len(aBuildingNotInCityRequirementList)):
				if aBuildingNotInCityRequirementList[i] in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" Not In City is replaced by blockers: "+GC.getBuildingInfo(aBuildingNotInCityRequirementList[i]).getType())
			#Check if we have blockers, that is replaced by building itself
			for i in xrange(len(aBuildingNotInCityRequirementList)):
				if aBuildingNotInCityRequirementList[i] in aBuildingReplacedList:
					self.log(CvBuildingInfo.getType()+" Not In City replaces blockers: "+GC.getBuildingInfo(aBuildingNotInCityRequirementList[i]).getType())
			#Check if we have blockers, that is replaced by unlisted blockers replacement, ignore blockers replacement, if it replaces currently checked building
			for i in xrange(len(aUnlistedRequirementReplacements)):
				if aUnlistedRequirementReplacements[i] not in aBuildingNotInCityRequirementList and aUnlistedRequirementReplacements[i] not in aBuildingReplacementList:
					self.log(CvBuildingInfo.getType()+" Not In City has unlisted replaced blockers: "+GC.getBuildingInfo(aUnlistedRequirementReplacements[i]).getType())

	#Building obsoletion of requirements - requirements shouldn't obsolete before building itself
	def checkBuildingRequirementObsoletion(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			BuildingObsoleteTechLoc = self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
			BuildingObsoleteTechID = self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[1]

			#<PrereqInCityBuildings> - require all buildings in list
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementObsoleteTechIDList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuilding)
				aBuildingRequirementObsoleteTechLocList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
				aBuildingRequirementNameList.append(GC.getBuildingInfo(iPrereqBuilding).getType())
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has AND requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList)-10 <= BuildingObsoleteTechLoc and min(aBuildingRequirementObsoleteTechLocList) < 999 and BuildingObsoleteTechID not in aBuildingRequirementObsoleteTechIDList:
				self.log(CvBuildingInfo.getType()+" has AND requirements obsolete fairly soon after base building - consider picking its obsoletion tech "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechIDList)+" "+str(BuildingObsoleteTechID))

			#<PrereqOrBuildings> - require one building in list
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementObsoleteTechIDList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuilding)
				aBuildingRequirementObsoleteTechLocList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
				aBuildingRequirementNameList.append(GC.getBuildingInfo(iPrereqBuilding).getType())
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has latest OR requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList)-10 <= BuildingObsoleteTechLoc and max(aBuildingRequirementObsoleteTechLocList) < 999 and BuildingObsoleteTechID not in aBuildingRequirementObsoleteTechIDList:
				self.log(CvBuildingInfo.getType()+" has latest OR requirements obsolete fairly soon after base building - consider picking its obsoletion tech "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechIDList)+" "+str(BuildingObsoleteTechID))

			#<PrereqAmountBuildings> - require all buildings in empire in list
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementObsoleteTechIDList = []
			aBuildingRequirementNameList = []
			for iPrereqBuilding, iNumRequired in CvBuildingInfo.getPrereqNumOfBuildings():
				aBuildingRequirementObsoleteTechLocList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
				aBuildingRequirementNameList.append(GC.getBuildingInfo(iPrereqBuilding).getType())
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has Empire AND requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList)-10 <= BuildingObsoleteTechLoc and min(aBuildingRequirementObsoleteTechLocList) < 999 and BuildingObsoleteTechID not in aBuildingRequirementObsoleteTechIDList:
				self.log(CvBuildingInfo.getType()+" has Empire AND requirements obsolete fairly soon after base building - consider picking its obsoletion tech "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechIDList)+" "+str(BuildingObsoleteTechID))

			#<ConstructCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.HF.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementObsoleteTechIDList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]
				aBuildingRequirementObsoleteTechLocList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
				aBuildingRequirementNameList.append(GC.getBuildingInfo(iPrereqBuilding).getType())
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has GOM AND requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList)-10 <= BuildingObsoleteTechLoc and min(aBuildingRequirementObsoleteTechLocList) < 999 and BuildingObsoleteTechID not in aBuildingRequirementObsoleteTechIDList:
				self.log(CvBuildingInfo.getType()+" has GOM AND requirements obsolete fairly soon after base building - consider picking its obsoletion tech "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechIDList)+" "+str(BuildingObsoleteTechID))

			#Analyze GOM OR Building reqs
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementObsoleteTechIDList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]
				aBuildingRequirementObsoleteTechLocList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
				aBuildingRequirementNameList.append(GC.getBuildingInfo(iPrereqBuilding).getType())
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has latest GOM OR requirement obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList)-10 <= BuildingObsoleteTechLoc and max(aBuildingRequirementObsoleteTechLocList) < 999 and BuildingObsoleteTechID not in aBuildingRequirementObsoleteTechIDList:
				self.log(CvBuildingInfo.getType()+" has latest GOM OR requirement obsolete fairly soon after base building - consider picking its obsoletion tech "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechIDList)+" "+str(BuildingObsoleteTechID))

			#<PrereqNotInCityBuildings> - Those block building from being built
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqNotInCityBuildings()):
				iBlockerBuilding = CvBuildingInfo.getPrereqNotInCityBuilding(iBuilding)
				CvBlockerBuilding = GC.getBuildingInfo(iBlockerBuilding)
				if BuildingObsoleteTechID != self.HF.checkBuildingTechObsoletionLocation(CvBlockerBuilding)[1]:
					self.log(CvBuildingInfo.getType()+" doesn't obsolete with "+CvBlockerBuilding.getType())

	#Buildings shouldn't obsolete too fast in relation of tech unlock
	def checkBuildingUnlockObsoletion(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]
			if CvBuildingInfo.getObsoleteTech() != -1:
				iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
				if iObsoleteTechLoc - iTechLoc <= 5:
					self.log(CvBuildingInfo.getType()+" Unlock: "+str(iTechLoc)+" Obsoletion: "+str(iObsoleteTechLoc)+" Difference: "+str(iObsoleteTechLoc - iTechLoc))

	#Building replacements shouldn't obsolete too fast for sanity of beeliners, replacements also shouldn't obsolete at earlier point compared to base
	def checkBuildingReplacementObsoletion(self):
		aSpecialReplacementsList = [GC.getInfoTypeForString("BUILDING_POLLUTION_BLACKENEDSKIES"), GC.getInfoTypeForString("BUILDING_GAMBLING_BAN"), GC.getInfoTypeForString("BUILDING_ALCOCHOL_PROHIBITION"), GC.getInfoTypeForString("BUILDING_DRUG_PROHIBITION"), GC.getInfoTypeForString("BUILDING_PROSTITUTION_BAN")]
		iEndOfTechTreeXGrid = GC.getTechInfo(GC.getInfoTypeForString("TECH_FUTURE_TECH")).getGridX()
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]
			iTechID = max(self.HF.checkBuildingTechRequirements(CvBuildingInfo)[2])
			iTechObsLoc = self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
			iTechObsID = self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[1]

			#All replacements of base
			aBuildingReplacementList = []
			for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				iBuildingReplacement = CvBuildingInfo.getReplacementBuilding(iReplacement)
				if iBuildingReplacement not in aSpecialReplacementsList and GC.getBuildingInfo(iBuildingReplacement).getType().find("BUILDING_EQ_") == -1 and GC.getBuildingInfo(iBuildingReplacement).getType().find("BUILDING_KNOWLEDGE_BASE_") == -1:
					aBuildingReplacementList.append(iBuildingReplacement)

			#All replacements of replacements
			aBuildingReplacement2List = []
			aBuildingReplacement2NameList = []
			for i in xrange(len(aBuildingReplacementList)):
				CvBuildingReplacementInfo = GC.getBuildingInfo(aBuildingReplacementList[i])
				for iReplacement2 in xrange(CvBuildingReplacementInfo.getNumReplacementBuilding()):
					iBuildingReplacement2 = CvBuildingReplacementInfo.getReplacementBuilding(iReplacement2)
					if iBuildingReplacement2 not in aBuildingReplacement2List and iBuildingReplacement2 not in aSpecialReplacementsList:
						aBuildingReplacement2List.append(iBuildingReplacement2)
						aBuildingReplacement2NameList.append(GC.getBuildingInfo(iBuildingReplacement2).getType())

			#Get replacements, that don't appear as replacements of replacements
			aImmediateReplacementList = []
			for i in xrange(len(aBuildingReplacementList)):
				if aBuildingReplacementList[i] not in aBuildingReplacement2List:
					CvBuildingReplacementInfo = GC.getBuildingInfo(aBuildingReplacementList[i])
					aImmediateReplacementList.append(aBuildingReplacementList[i])

			#There may be several immediate replacements
			if len(aImmediateReplacementList) > 0:
				for i in xrange(len(aImmediateReplacementList)):
					CvBuildingImmediateReplacementInfo = GC.getBuildingInfo(aImmediateReplacementList[i])
					iImmediateReplacementTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingImmediateReplacementInfo)[0]
					iImmediateReplacementTechLocID = max(self.HF.checkBuildingTechRequirements(CvBuildingImmediateReplacementInfo)[2])
					iImmediateReplacementTechObs = self.HF.checkBuildingTechObsoletionLocation(CvBuildingImmediateReplacementInfo)[0]
					iImmediateReplacementTechObsID = self.HF.checkBuildingTechObsoletionLocation(CvBuildingImmediateReplacementInfo)[1]
					if iBuilding != GC.getInfoTypeForString("BUILDING_HOMELESS"):
						if iTechLoc >= iImmediateReplacementTechLoc:
							self.log(CvBuildingInfo.getType()+" unlocks after or concurrently with unlock of "+CvBuildingImmediateReplacementInfo.getType()+" Base unlock/Replacement unlock "+str(iTechLoc)+"/"+str(iImmediateReplacementTechLoc))
						if iTechObsLoc > iImmediateReplacementTechObs:
							self.log(CvBuildingInfo.getType()+" obsoletes after obsoletion of "+CvBuildingImmediateReplacementInfo.getType()+" Base obsoletion/Replacement obsoletion "+str(iTechObsLoc)+"/"+str(iImmediateReplacementTechObs))

			#Get replacements of replacements
			for i in xrange(len(aImmediateReplacementList)):
				Cv2BuildingInfo = GC.getBuildingInfo(aImmediateReplacementList[i])
				iReplacementTechLoc = self.HF.checkBuildingTechRequirements(Cv2BuildingInfo)[0]

				#All replacements of replacements
				a2BuildingReplacementList = []
				for iReplacement in xrange(Cv2BuildingInfo.getNumReplacementBuilding()):
					iBuildingReplacement = Cv2BuildingInfo.getReplacementBuilding(iReplacement)
					if iBuildingReplacement not in aSpecialReplacementsList:
						a2BuildingReplacementList.append(iBuildingReplacement)

				#All third level replacements
				a2BuildingReplacement2List = []
				for i in xrange(len(a2BuildingReplacementList)):
					Cv2BuildingReplacementInfo = GC.getBuildingInfo(a2BuildingReplacementList[i])
					for iReplacement2 in xrange(Cv2BuildingReplacementInfo.getNumReplacementBuilding()):
						iBuildingReplacement2 = Cv2BuildingReplacementInfo.getReplacementBuilding(iReplacement2)
						if iBuildingReplacement2 not in a2BuildingReplacement2List and iBuildingReplacement2 not in aSpecialReplacementsList:
							a2BuildingReplacement2List.append(iBuildingReplacement2)

				#Get replacements, that don't appear as third level replacements
				aImmediateReplacement2List = []
				for i in xrange(len(a2BuildingReplacementList)):
					if a2BuildingReplacementList[i] not in a2BuildingReplacement2List:
						Cv2BuildingReplacementInfo = GC.getBuildingInfo(a2BuildingReplacementList[i])
						aImmediateReplacement2List.append(a2BuildingReplacementList[i])

				if len(aImmediateReplacement2List) > 0:
					for i in xrange(len(aImmediateReplacement2List)):
						Cv2BuildingImmediateReplacementInfo = GC.getBuildingInfo(aImmediateReplacement2List[i])
						iReplacement2TechID = max(self.HF.checkBuildingTechRequirements(Cv2BuildingImmediateReplacementInfo)[2])
						if iTechObsID != iReplacement2TechID and iBuilding != GC.getInfoTypeForString("BUILDING_HOMELESS"):
							self.log(CvBuildingInfo.getType()+" -> "+Cv2BuildingImmediateReplacementInfo.getType()+" Base Obsoletion should happen on Second Replacement Unlock: "+self.HF.getTechName(iTechObsID)+"/"+self.HF.getTechName(iReplacement2TechID))

				if len(aImmediateReplacementList) == 1 and len(aImmediateReplacement2List) == 1 and CvBuildingInfo.getObsoletesToBuilding() != aImmediateReplacementList[0]:
					if not GC.getBuildingInfo(aImmediateReplacementList[0]).isAutoBuild() and not GC.getBuildingInfo(aImmediateReplacement2List[0]).isAutoBuild():
						self.log(CvBuildingInfo.getType()+" should obsolete into "+GC.getBuildingInfo(aImmediateReplacementList[0]).getType()+" as its obsoleted on unlock of "+GC.getBuildingInfo(aImmediateReplacement2List[0]).getType())
				if len(aImmediateReplacementList) == 1 and len(aImmediateReplacement2List) == 0 and CvBuildingInfo.getObsoletesToBuilding() != aImmediateReplacementList[0]:
					if CvBuildingInfo.getProductionCost() > 0 and GC.getBuildingInfo(aImmediateReplacementList[0]).getProductionCost() > 0:
						self.log(CvBuildingInfo.getType()+" should obsolete into "+GC.getBuildingInfo(aImmediateReplacementList[0]).getType()+" when it obsoletes")
				if CvBuildingInfo.getObsoletesToBuilding() != -1 and (len(aImmediateReplacementList) > 1 or len(aImmediateReplacement2List) > 1):
					self.log("WARNING: "+CvBuildingInfo.getType()+" has multiple first or second level replacements and shouldn't obsolete to building")

		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]

			#Ignore Pollution, and Bans
			if iBuilding not in aSpecialReplacementsList and CvBuildingInfo.getNumReplacedBuilding() != 0:
				#Get list of replaced buildings
				aReplacedBuildings = []
				for i in xrange(CvBuildingInfo.getNumReplacedBuilding()):
					aReplacedBuildings.append(CvBuildingInfo.getReplacedBuilding(i))

				#All replaced buildings of replaced ones
				aBuildingReplaced2List = []
				for i in xrange(len(aReplacedBuildings)):
					CvBuildingReplacedInfo = GC.getBuildingInfo(aReplacedBuildings[i])
					for iReplaced2 in xrange(CvBuildingReplacedInfo.getNumReplacedBuilding()):
						iBuildingReplacd2 = CvBuildingReplacedInfo.getReplacedBuilding(iReplaced2)
						if iBuildingReplacd2 not in aBuildingReplaced2List:
							aBuildingReplaced2List.append(iBuildingReplacd2)

				#Get replacements, that don't appear as replaced of replaced.
				aImmediateReplacedList = []
				aImmediateReplacedObsoletionList = []
				aImmediateReplacedObsLocList = []
				aImmediateReplacedNameList = []
				for i in xrange(len(aReplacedBuildings)):
					if aReplacedBuildings[i] not in aBuildingReplaced2List:
						CvBuildingReplacedInfo = GC.getBuildingInfo(aReplacedBuildings[i])
						aImmediateReplacedList.append(aReplacedBuildings[i])
						aImmediateReplacedObsoletionList.append(self.HF.checkBuildingTechObsoletionLocation(CvBuildingReplacedInfo)[1])
						aImmediateReplacedObsLocList.append(self.HF.checkBuildingTechObsoletionLocation(CvBuildingReplacedInfo)[0])
						aImmediateReplacedNameList.append(GC.getBuildingInfo(aReplacedBuildings[i]).getType())

				if CvBuildingInfo.getNumReplacementBuilding() == 0 and len(aImmediateReplacedList) > 0: #Checked building is last in line
					for i in xrange(len(aImmediateReplacedList)):
						if aImmediateReplacedObsoletionList[i] > min(aImmediateReplacedObsoletionList):
							self.log(CvBuildingInfo.getType()+" replaced building "+aImmediateReplacedNameList[i]+" should obsolete at "+self.HF.getTechName(min(aImmediateReplacedObsoletionList)))
						if min(aImmediateReplacedObsLocList) - iTechLoc < 10 and min(aImmediateReplacedObsLocList) != iEndOfTechTreeXGrid - 1: #At least 10 columns between second last building obsoletion and last building unlock, can't move further buildings obsoleteing at end of history
							self.log(CvBuildingInfo.getType()+" replaced building "+aImmediateReplacedNameList[i]+" last unlock/second last obsoletion "+str(iTechLoc)+"/"+str(min(aImmediateReplacedObsLocList)))

	#Building - Check if we don't have implicit replacements, also ensure that listed ones aren't unlocked before building
	def checkBuildingImplicitReplacements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iBaseTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]
			iBaseObsLoc = self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]

			#Replacements
			aReplacementBuildingList = []
			aReplacementBuildingTechLocList = []
			for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				iReplacementBuilding = CvBuildingInfo.getReplacementBuilding(i)
				CvBuildingReplacementInfo = GC.getBuildingInfo(iReplacementBuilding)
				aReplacementBuildingList.append(iReplacementBuilding)
				iReplacementTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingReplacementInfo)[0]
				iReplacementObsLoc = self.HF.checkBuildingTechObsoletionLocation(CvBuildingReplacementInfo)[0]
				if iBaseTechLoc == iReplacementTechLoc and CvBuildingReplacementInfo.getProductionCost() > 0: #Concurrent replacement is fine, if it can't be built normally
					self.log("WARNING: "+CvBuildingInfo.getType()+" is unlocked concurrently with "+CvBuildingReplacementInfo.getType()+" "+str(iBaseTechLoc)+"/"+str(iReplacementTechLoc))
				if iBaseObsLoc == iReplacementObsLoc and CvBuildingInfo.getProductionCost() > 0 and CvBuildingReplacementInfo.getProductionCost() > 0: #Concurrent obsoletion is fine, if it can't be built normally
					self.log("WARNING: "+CvBuildingInfo.getType()+" is obsoleted concurrently with "+CvBuildingReplacementInfo.getType()+" "+str(iBaseObsLoc)+"/"+str(iReplacementObsLoc))

			#Replacements of replacements
			aReplacement2BuildingList = []
			for i in xrange(len(aReplacementBuildingList)):
				CvBuildingReplacementInfo = GC.getBuildingInfo(aReplacementBuildingList[i])
				for j in xrange(CvBuildingReplacementInfo.getNumReplacementBuilding()):
					iReplacement2Building = CvBuildingReplacementInfo.getReplacementBuilding(j)
					aReplacement2BuildingList.append(iReplacement2Building)

			#Get building type names
			aReplacementBuildingTypeList = []
			for i in xrange(len(aReplacementBuildingList)):
				aReplacementBuildingTypeList.append(GC.getBuildingInfo(aReplacementBuildingList[i]).getType())
			aReplacement2BuildingTypeList = []
			for i in xrange(len(aReplacement2BuildingList)):
				aReplacement2BuildingTypeList.append(GC.getBuildingInfo(aReplacement2BuildingList[i]).getType())
			aImplicitReplacementList = []
			for i in xrange(len(aReplacement2BuildingTypeList)):
				if aReplacement2BuildingTypeList[i] not in aReplacementBuildingTypeList:
					aImplicitReplacementList.append(aReplacement2BuildingTypeList[i])
			aImplicitReplacementUniqueList = []
			for i in xrange(len(aImplicitReplacementList)):
				if aImplicitReplacementList[i] not in aImplicitReplacementUniqueList:
					aImplicitReplacementUniqueList.append(aImplicitReplacementList[i])

			#Building can't have replacements, that are implicit - that is unlisted replacements of replacements
			if len(aReplacementBuildingList) > 0 and len(aImplicitReplacementList) > 0:
				if CvBuildingInfo.getNumReplacedBuilding() == 0:
					self.log("BASE: "+CvBuildingInfo.getType()+" -> "+str(aReplacementBuildingTypeList)+" implicit: "+str(aImplicitReplacementUniqueList))
				else:
					self.log(CvBuildingInfo.getType()+" -> "+str(aReplacementBuildingTypeList)+" implicit: "+str(aImplicitReplacementUniqueList))

	#Building - check if replacing building has better yields, commerces, and other stats
	def checkBuildingReplacingQuality(self):
		aSpecialBuildingsList = [GC.getInfoTypeForString("BUILDING_POLLUTION_BLACKENEDSKIES"), GC.getInfoTypeForString("BUILDING_GAMBLING_BAN"), GC.getInfoTypeForString("BUILDING_ALCOCHOL_PROHIBITION"), GC.getInfoTypeForString("BUILDING_DRUG_PROHIBITION"), GC.getInfoTypeForString("BUILDING_PROSTITUTION_BAN")]
		aCorporateBonusList = []
		for iCorporation in xrange(GC.getNumCorporationInfos()):
			CvCorporationInfo = GC.getCorporationInfo(iCorporation)
			for iBonus in CvCorporationInfo.getPrereqBonuses():
				if iBonus not in aCorporateBonusList:
					aCorporateBonusList.append(iBonus)

		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			#Tech location would be good way to sort replacements, as later ones tend to replace more
			iTechID = max(self.HF.checkBuildingTechRequirements(CvBuildingInfo)[2])
			iTechBase = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0] #Tech level of building - most advanced tech XGrid

			#Ignore Pollution, Bans and Education pseudobuildings
			if iBuilding not in aSpecialBuildingsList and CvBuildingInfo.getNumReplacedBuilding() != 0 and CvBuildingInfo.getType().find("BUILDING_EQ_") == -1:
				#Get list of replaced buildings
				aReplacedBuildings = []
				for i in xrange(CvBuildingInfo.getNumReplacedBuilding()):
					aReplacedBuildings.append(CvBuildingInfo.getReplacedBuilding(i))

				#All replaced buildings of replaced ones
				aBuildingReplaced2List = []
				for i in xrange(len(aReplacedBuildings)):
					CvBuildingReplacedInfo = GC.getBuildingInfo(aReplacedBuildings[i])
					for iReplaced2 in xrange(CvBuildingReplacedInfo.getNumReplacedBuilding()):
						iBuildingReplacd2 = CvBuildingReplacedInfo.getReplacedBuilding(iReplaced2)
						if iBuildingReplacd2 not in aBuildingReplaced2List:
							aBuildingReplaced2List.append(iBuildingReplacd2)

				#Get replacements, that don't appear as replaced of replaced.
				aImmediateReplacedList = []
				for i in xrange(len(aReplacedBuildings)):
					if aReplacedBuildings[i] not in aBuildingReplaced2List:
						CvBuildingReplacedInfo = GC.getBuildingInfo(aReplacedBuildings[i])
						aImmediateReplacedList.append(aReplacedBuildings[i])

				#aTag = [BASE, REPLACED]
				#BASE - values/arrays of building, that is currently checked
				#REPLACED - values/arrays of buildings, that got replaced by checked building
				BASE = 0
				REPLACED = 1
				MAIN_ARRAY_SIZE = 2

				#===== 0D ENTRIES - INTEGERS ===========================================================================================================================#
				#<bPower>, <bProvidesFreshWater>, <iTradeRoutes>, <iCoastalTradeRoutes>, <iGlobalTradeRoutes>, <iTradeRouteModifier>, <iForeignTradeRouteModifier>, <iHappiness>, <iHealth>, <iGreatPeopleRateChange>, <iGreatPeopleRateModifier>, <iFreeSpecialist>, <iAreaFreeSpecialist>, <iGlobalFreeSpecialist>, <iMaintenanceModifier>, <iHappinessPercentPerPopulation>, <iHealthPercentPerPopulation>, <iWarWearinessModifier>, <iGlobalWarWearinessModifier>, <iEnemyWarWearinessModifier>, <iAllCityDefense>, <iBombardDefense>, <iBuildingDefenseRecoverySpeedModifier>, <iCityDefenseRecoverySpeedModifier>, <iDefense>, <iEspionageDefense>, <iLocalDynamicDefense>, <iMinDefense>, <iNoEntryDefenseLevel>, <iRiverDefensePenalty>, <iExperience>, <iGlobalExperience>, <iFoodKept> <iPopulationgrowthratepercentage>, <iAdjacentDamagePercent>, <iLineOfSight>, <iAirModifier>, <iAirUnitCapacity>, <iAirlift>, <iAnarchyModifier>, <iAreaHappiness>, <iAreaHealth>, <iDamageAttackerChance>, <iDamageToAttacker>, <iDistanceMaintenanceModifier>, <iDomesticGreatGeneralRateModifier>, <iGlobalGreatPeopleRateModifier>, <iGlobalHappiness>, <iGlobalHealth>, <iGlobalMaintenanceModifier>, <iGlobalSpaceProductionModifier>, <iGoldenAgeModifier>, <iGreatGeneralRateModifier>, <iHealRateChange>, <iInsidiousness>, <iInvasionChance>, <iInvestigation>, <iLocalRepel>, <iMilitaryProductionModifier>, <iNationalCaptureProbabilityModifier>, <iNationalCaptureResistanceModifier>, <iNukeModifier>, <iNumUnitFullHeal>, <iRevIdxLocal>, <iRevIdxNational>, <iSpaceProductionModifier>, <iStateReligionHappiness>, <iUnitUpgradePriceModifier>, <iWorkerSpeedModifier> - base
				aPower = [CvBuildingInfo.isPower(), 0]
				aProvidesFreshWater = [CvBuildingInfo.isProvidesFreshWater(), 0]
				aTradeRoutes = [CvBuildingInfo.getTradeRoutes(), 0]
				aCoastalTradeRoutes = [CvBuildingInfo.getCoastalTradeRoutes(), 0]
				aGlobalTradeRoutes = [CvBuildingInfo.getGlobalTradeRoutes(), 0]
				aTradeRouteModifier = [CvBuildingInfo.getTradeRouteModifier(), 0]
				aForeignTradeRouteModifier = [CvBuildingInfo.getForeignTradeRouteModifier(), 0]
				aHappiness = [CvBuildingInfo.getHappiness(), 0]
				aHealth = [CvBuildingInfo.getHealth(), 0]
				aGreatPeopleRateChange = [CvBuildingInfo.getGreatPeopleRateChange(), 0]
				aGreatPeopleRateModifier = [CvBuildingInfo.getGreatPeopleRateModifier(), 0]
				aFreeSpecialist = [CvBuildingInfo.getFreeSpecialist(), 0]
				aAreaFreeSpecialist = [CvBuildingInfo.getAreaFreeSpecialist(), 0]
				aGlobalFreeSpecialist = [CvBuildingInfo.getGlobalFreeSpecialist(), 0]
				aMaintenanceModifier = [CvBuildingInfo.getMaintenanceModifier(), 0]  #iMaintenanceModifier - lower value is better
				aHappinessPercentPerPopulation = [CvBuildingInfo.getHappinessPercentPerPopulation(), 0]
				aHealthPercentPerPopulation = [CvBuildingInfo.getHealthPercentPerPopulation(), 0]
				aWarWearinessModifier = [CvBuildingInfo.getWarWearinessModifier(), 0]  #iWarWearinessModifier - lower value is better
				aGlobalWarWearinessModifier = [CvBuildingInfo.getGlobalWarWearinessModifier(), 0]  #iGlobalWarWearinessModifier - lower value is better
				aEnemyWarWearinessModifier = [CvBuildingInfo.getEnemyWarWearinessModifier(), 0]
				aAllCityDefense = [CvBuildingInfo.getAllCityDefenseModifier(), 0]
				aBombardDefense = [CvBuildingInfo.getBombardDefenseModifier(), 0]
				aBuildingDefenseRecoverySpeedModifier = [CvBuildingInfo.getBuildingDefenseRecoverySpeedModifier(), 0]
				aCityDefenseRecoverySpeedModifier = [CvBuildingInfo.getCityDefenseRecoverySpeedModifier(), 0]
				aDefense = [CvBuildingInfo.getDefenseModifier(), 0]
				aEspionageDefense = [CvBuildingInfo.getEspionageDefenseModifier(), 0]
				aLocalDynamicDefense = [CvBuildingInfo.getLocalDynamicDefense(), 0]
				aMinDefense = [CvBuildingInfo.getMinDefense(), 0]
				aNoEntryDefenseLevel = [CvBuildingInfo.getNoEntryDefenseLevel(), 0]   #iNoEntryDefenseLevel - lower value is better
				aRiverDefensePenalty = [CvBuildingInfo.getRiverDefensePenalty(), 0]
				aExperience = [CvBuildingInfo.getFreeExperience(), 0]
				aGlobalExperience = [CvBuildingInfo.getGlobalFreeExperience(), 0]
				aFoodKept = [CvBuildingInfo.getFoodKept(), 0]
				aPopulationgrowthratepercentage = [CvBuildingInfo.getPopulationgrowthratepercentage(), 0]
				aAdjacentDamagePercent = [CvBuildingInfo.getAdjacentDamagePercent(), 0]
				aLineOfSight = [CvBuildingInfo.getLineOfSight(), 0]
				aAirModifier = [CvBuildingInfo.getAirModifier(), 0]
				aAirUnitCapacity = [CvBuildingInfo.getAirUnitCapacity(), 0]
				aAirlift = [CvBuildingInfo.getAirlift(), 0]
				aAnarchyModifier = [CvBuildingInfo.getAnarchyModifier(), 0]
				aAreaHappiness = [CvBuildingInfo.getAreaHappiness(), 0]
				aAreaHealth = [CvBuildingInfo.getAreaHealth(), 0]
				aDamageAttackerChance = [CvBuildingInfo.getDamageAttackerChance(), 0]
				aDamageToAttacker = [CvBuildingInfo.getDamageToAttacker(), 0]
				aDistanceMaintenanceModifier = [CvBuildingInfo.getDistanceMaintenanceModifier(), 0]
				aDomesticGreatGeneralRateModifier = [CvBuildingInfo.getDomesticGreatGeneralRateModifier(), 0]
				aGlobalGreatPeopleRateModifier = [CvBuildingInfo.getGlobalGreatPeopleRateModifier(), 0]
				aGlobalHappiness = [CvBuildingInfo.getGlobalHappiness(), 0]
				aGlobalHealth = [CvBuildingInfo.getGlobalHealth(), 0]
				aGlobalMaintenanceModifier = [CvBuildingInfo.getGlobalMaintenanceModifier(), 0]
				aGlobalSpaceProductionModifier = [CvBuildingInfo.getGlobalSpaceProductionModifier(), 0]
				aGoldenAgeModifier = [CvBuildingInfo.getGoldenAgeModifier(), 0]
				aGreatGeneralRateModifier = [CvBuildingInfo.getGreatGeneralRateModifier(), 0]
				aHealRateChange = [CvBuildingInfo.getHealRateChange(), 0]
				aInsidiousness = [CvBuildingInfo.getInsidiousness(), 0]
				aInvasionChance = [CvBuildingInfo.getInvasionChance(), 0]
				aInvestigation = [CvBuildingInfo.getInvestigation(), 0]
				aLocalRepel = [CvBuildingInfo.getLocalRepel(), 0]
				aMilitaryProductionModifier = [CvBuildingInfo.getMilitaryProductionModifier(), 0]
				aNationalCaptureProbabilityModifier = [CvBuildingInfo.getNationalCaptureProbabilityModifier(), 0]
				aNationalCaptureResistanceModifier = [CvBuildingInfo.getNationalCaptureResistanceModifier(), 0]
				aNukeModifier = [CvBuildingInfo.getNukeModifier(), 0]
				aNumUnitFullHeal = [CvBuildingInfo.getNumUnitFullHeal(), 0]
				aRevIdxLocal = [CvBuildingInfo.getRevIdxLocal(), 0]
				aRevIdxNational = [CvBuildingInfo.getRevIdxNational(), 0]
				aSpaceProductionModifier = [CvBuildingInfo.getSpaceProductionModifier(), 0]
				aStateReligionHappiness = [CvBuildingInfo.getStateReligionHappiness(), 0]
				aUnitUpgradePriceModifier = [CvBuildingInfo.getUnitUpgradePriceModifier(), 0]
				aWorkerSpeedModifier = [CvBuildingInfo.getWorkerSpeedModifier(), 0]

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<bPower>, <bProvidesFreshWater>, <iTradeRoutes>, <iCoastalTradeRoutes>, <iGlobalTradeRoutes>, <iTradeRouteModifier>, <iForeignTradeRouteModifier>, <iHappiness>, <iHealth>, <iGreatPeopleRateChange>, <iGreatPeopleRateModifier>, <iFreeSpecialist>, <iAreaFreeSpecialist>, <iGlobalFreeSpecialist>, <iMaintenanceModifier>, <iHappinessPercentPerPopulation>, <iHealthPercentPerPopulation>, <iWarWearinessModifier>, <iGlobalWarWearinessModifier>, <iEnemyWarWearinessModifier>, <iAllCityDefense>, <iBombardDefense>, <iBuildingDefenseRecoverySpeedModifier>, <iCityDefenseRecoverySpeedModifier>, <iDefense>, <iEspionageDefense>, <iLocalDynamicDefense>, <iMinDefense>, <iNoEntryDefenseLevel>, <iRiverDefensePenalty>, <iExperience>, <iGlobalExperience>, <iFoodKept> <iPopulationgrowthratepercentage>, <iAdjacentDamagePercent>, <iLineOfSight>, <iAirModifier>, <iAirUnitCapacity>, <iAirlift>, <iAnarchyModifier>, <iAreaHappiness>, <iAreaHealth>, <iDamageAttackerChance>, <iDamageToAttacker>, <iDistanceMaintenanceModifier>, <iDomesticGreatGeneralRateModifier>, <iGlobalGreatPeopleRateModifier>, <iGlobalHappiness>, <iGlobalHealth>, <iGlobalMaintenanceModifier>, <iGlobalSpaceProductionModifier>, <iGoldenAgeModifier>, <iGreatGeneralRateModifier>, <iHealRateChange>, <iInsidiousness>, <iInvasionChance>, <iInvestigation>, <iLocalRepel>, <iMilitaryProductionModifier>, <iNationalCaptureProbabilityModifier>, <iNationalCaptureResistanceModifier>, <iNukeModifier>, <iNumUnitFullHeal>, <iRevIdxLocal>, <iRevIdxNational>, <iSpaceProductionModifier>, <iStateReligionHappiness>, <iUnitUpgradePriceModifier>, <iWorkerSpeedModifier>
					aPower[REPLACED] += CvReplacedBuildingInfo.isPower()
					aProvidesFreshWater[REPLACED] += CvReplacedBuildingInfo.isProvidesFreshWater()
					aTradeRoutes[REPLACED] += CvReplacedBuildingInfo.getTradeRoutes()
					aCoastalTradeRoutes[REPLACED] += CvReplacedBuildingInfo.getCoastalTradeRoutes()
					aGlobalTradeRoutes[REPLACED] += CvReplacedBuildingInfo.getGlobalTradeRoutes()
					aTradeRouteModifier[REPLACED] += CvReplacedBuildingInfo.getTradeRouteModifier()
					aForeignTradeRouteModifier[REPLACED] += CvReplacedBuildingInfo.getForeignTradeRouteModifier()
					aHappiness[REPLACED] += CvReplacedBuildingInfo.getHappiness()
					aHealth[REPLACED] += CvReplacedBuildingInfo.getHealth()
					aGreatPeopleRateChange[REPLACED] += CvReplacedBuildingInfo.getGreatPeopleRateChange()
					if CvBuildingInfo.getGreatPeopleUnitType() != CvReplacedBuildingInfo.getGreatPeopleUnitType() and CvReplacedBuildingInfo.getGreatPeopleUnitType() != -1:
						self.log("WARNING: "+CvBuildingInfo.getType()+", "+CvReplacedBuildingInfo.getType()+" have different GPs, remove GPUnitType from both buildings, or unify entire replacement tree")
					aGreatPeopleRateModifier[REPLACED] += CvReplacedBuildingInfo.getGreatPeopleRateModifier()
					aFreeSpecialist[REPLACED] += CvReplacedBuildingInfo.getFreeSpecialist()
					aAreaFreeSpecialist[REPLACED] += CvReplacedBuildingInfo.getAreaFreeSpecialist()
					aGlobalFreeSpecialist[REPLACED] += CvReplacedBuildingInfo.getGlobalFreeSpecialist()
					aMaintenanceModifier[REPLACED] += CvReplacedBuildingInfo.getMaintenanceModifier() #iMaintenanceModifier - lower value is better
					aHappinessPercentPerPopulation[REPLACED] += CvReplacedBuildingInfo.getHappinessPercentPerPopulation()
					aHealthPercentPerPopulation[REPLACED] += CvReplacedBuildingInfo.getHealthPercentPerPopulation()
					aWarWearinessModifier[REPLACED] += CvReplacedBuildingInfo.getWarWearinessModifier() #iWarWearinessModifier - lower value is better
					aGlobalWarWearinessModifier[REPLACED] += CvReplacedBuildingInfo.getGlobalWarWearinessModifier() #iGlobalWarWearinessModifier - lower value is better
					aEnemyWarWearinessModifier[REPLACED] += CvReplacedBuildingInfo.getEnemyWarWearinessModifier()
					aAllCityDefense[REPLACED] += CvReplacedBuildingInfo.getAllCityDefenseModifier()
					aBombardDefense[REPLACED] += CvReplacedBuildingInfo.getBombardDefenseModifier()
					aBuildingDefenseRecoverySpeedModifier[REPLACED] += CvReplacedBuildingInfo.getBuildingDefenseRecoverySpeedModifier()
					aCityDefenseRecoverySpeedModifier[REPLACED] += CvReplacedBuildingInfo.getCityDefenseRecoverySpeedModifier()
					aDefense[REPLACED] += CvReplacedBuildingInfo.getDefenseModifier()
					aEspionageDefense[REPLACED] += CvReplacedBuildingInfo.getEspionageDefenseModifier()
					aLocalDynamicDefense[REPLACED] += CvReplacedBuildingInfo.getLocalDynamicDefense()
					aMinDefense[REPLACED] += CvReplacedBuildingInfo.getMinDefense()
					aNoEntryDefenseLevel[REPLACED] += CvReplacedBuildingInfo.getNoEntryDefenseLevel()  #iNoEntryDefenseLevel - lower value is better
					aRiverDefensePenalty[REPLACED] += CvReplacedBuildingInfo.getRiverDefensePenalty()
					aExperience[REPLACED] += CvReplacedBuildingInfo.getFreeExperience()
					aGlobalExperience[REPLACED] += CvReplacedBuildingInfo.getGlobalFreeExperience()
					aFoodKept[REPLACED] += CvReplacedBuildingInfo.getFoodKept()
					aPopulationgrowthratepercentage[REPLACED] += CvReplacedBuildingInfo.getPopulationgrowthratepercentage()
					aAdjacentDamagePercent[REPLACED] += CvReplacedBuildingInfo.getAdjacentDamagePercent()
					aLineOfSight[REPLACED] += CvReplacedBuildingInfo.getLineOfSight()
					aAirModifier[REPLACED] += CvReplacedBuildingInfo.getAirModifier()
					aAirUnitCapacity[REPLACED] += CvReplacedBuildingInfo.getAirUnitCapacity()
					aAirlift[REPLACED] += CvReplacedBuildingInfo.getAirlift()
					aAnarchyModifier[REPLACED] += CvReplacedBuildingInfo.getAnarchyModifier()
					aAreaHappiness[REPLACED] += CvReplacedBuildingInfo.getAreaHappiness()
					aAreaHealth[REPLACED] += CvReplacedBuildingInfo.getAreaHealth()
					aDamageAttackerChance[REPLACED] += CvReplacedBuildingInfo.getDamageAttackerChance()
					aDamageToAttacker[REPLACED] += CvReplacedBuildingInfo.getDamageToAttacker()
					aDistanceMaintenanceModifier[REPLACED] += CvReplacedBuildingInfo.getDistanceMaintenanceModifier()
					aDomesticGreatGeneralRateModifier[REPLACED] += CvReplacedBuildingInfo.getDomesticGreatGeneralRateModifier()
					aGlobalGreatPeopleRateModifier[REPLACED] += CvReplacedBuildingInfo.getGlobalGreatPeopleRateModifier()
					aGlobalHappiness[REPLACED] += CvReplacedBuildingInfo.getGlobalHappiness()
					aGlobalHealth[REPLACED] += CvReplacedBuildingInfo.getGlobalHealth()
					aGlobalMaintenanceModifier[REPLACED] += CvReplacedBuildingInfo.getGlobalMaintenanceModifier()
					aGlobalSpaceProductionModifier[REPLACED] += CvReplacedBuildingInfo.getGlobalSpaceProductionModifier()
					aGoldenAgeModifier[REPLACED] += CvReplacedBuildingInfo.getGoldenAgeModifier()
					aGreatGeneralRateModifier[REPLACED] += CvReplacedBuildingInfo.getGreatGeneralRateModifier()
					aHealRateChange[REPLACED] += CvReplacedBuildingInfo.getHealRateChange()
					aInsidiousness[REPLACED] += CvReplacedBuildingInfo.getInsidiousness()
					aInvasionChance[REPLACED] += CvReplacedBuildingInfo.getInvasionChance()
					aInvestigation[REPLACED] += CvReplacedBuildingInfo.getInvestigation()
					aLocalRepel[REPLACED] += CvReplacedBuildingInfo.getLocalRepel()
					aMilitaryProductionModifier[REPLACED] += CvReplacedBuildingInfo.getMilitaryProductionModifier()
					aNationalCaptureProbabilityModifier[REPLACED] += CvReplacedBuildingInfo.getNationalCaptureProbabilityModifier()
					aNationalCaptureResistanceModifier[REPLACED] += CvReplacedBuildingInfo.getNationalCaptureResistanceModifier()
					aNukeModifier[REPLACED] += CvReplacedBuildingInfo.getNukeModifier()
					aNumUnitFullHeal[REPLACED] += CvReplacedBuildingInfo.getNumUnitFullHeal()
					aRevIdxLocal[REPLACED] += CvReplacedBuildingInfo.getRevIdxLocal()
					aRevIdxNational[REPLACED] += CvReplacedBuildingInfo.getRevIdxNational()
					aSpaceProductionModifier[REPLACED] += CvReplacedBuildingInfo.getSpaceProductionModifier()
					aStateReligionHappiness[REPLACED] += CvReplacedBuildingInfo.getStateReligionHappiness()
					aUnitUpgradePriceModifier[REPLACED] += CvReplacedBuildingInfo.getUnitUpgradePriceModifier()
					aWorkerSpeedModifier[REPLACED] += CvReplacedBuildingInfo.getWorkerSpeedModifier()

				#Building shouldn't be worse than replaced one!
				if aPower[BASE] == 0 and aPower[REPLACED] != 0:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should provide Power")
				if aProvidesFreshWater[BASE] == 0 and aProvidesFreshWater[REPLACED] != 0:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should provide Fresh Water")
				if aTradeRoutes[BASE] < aTradeRoutes[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Trade Routes "+str(aTradeRoutes[BASE])+"/"+str(aTradeRoutes[REPLACED]))
				if aCoastalTradeRoutes[BASE] < aCoastalTradeRoutes[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Coastal Trade Routes "+str(aCoastalTradeRoutes[BASE])+"/"+str(aCoastalTradeRoutes[REPLACED]))
				if aGlobalTradeRoutes[BASE] < aGlobalTradeRoutes[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global Trade Routes "+str(aGlobalTradeRoutes[BASE])+"/"+str(aGlobalTradeRoutes[REPLACED]))
				if aTradeRouteModifier[BASE] < aTradeRouteModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Trade Route Modifier "+str(aTradeRouteModifier[BASE])+"/"+str(aTradeRouteModifier[REPLACED]))
				if aForeignTradeRouteModifier[BASE] < aForeignTradeRouteModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Foreign Trade Route Modifier "+str(aForeignTradeRouteModifier[BASE])+"/"+str(aForeignTradeRouteModifier[REPLACED]))
				if aHappiness[BASE] < aHappiness[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Happiness "+str(aHappiness[BASE])+"/"+str(aHappiness[REPLACED]))
				if aHealth[BASE] < aHealth[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Health "+str(aHealth[BASE])+"/"+str(aHealth[REPLACED]))
				if aGreatPeopleRateChange[BASE] < aGreatPeopleRateChange[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have GP Rate Change "+str(aGreatPeopleRateChange[BASE])+"/"+str(aGreatPeopleRateChange[REPLACED]))
				if aGreatPeopleRateModifier[BASE] < aGreatPeopleRateModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have GP Rate Modifier "+str(aGreatPeopleRateModifier[BASE])+"/"+str(aGreatPeopleRateModifier[REPLACED]))
				if aFreeSpecialist[BASE] < aFreeSpecialist[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Free Specialist "+str(aFreeSpecialist[BASE])+"/"+str(aFreeSpecialist[REPLACED]))
				if aAreaFreeSpecialist[BASE] < aAreaFreeSpecialist[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Area free Specialist "+str(aAreaFreeSpecialist[BASE])+"/"+str(aAreaFreeSpecialist[REPLACED]))
				if aGlobalFreeSpecialist[BASE] < aGlobalFreeSpecialist[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global free Specialist "+str(aGlobalFreeSpecialist[BASE])+"/"+str(aGlobalFreeSpecialist[REPLACED]))
				if aMaintenanceModifier[BASE] > aMaintenanceModifier[REPLACED]: #iMaintenanceModifier - lower value is better, as it increases cost of running city
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Maintaince Modifier "+str(aMaintenanceModifier[BASE])+"/"+str(aMaintenanceModifier[REPLACED]))
				if aHappinessPercentPerPopulation[BASE] < aHappinessPercentPerPopulation[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Happiness per Pop "+str(aHappinessPercentPerPopulation[BASE])+"/"+str(aHappinessPercentPerPopulation[REPLACED]))
				if aHealthPercentPerPopulation[BASE] < aHealthPercentPerPopulation[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Health per Pop "+str(aHealthPercentPerPopulation[BASE])+"/"+str(aHealthPercentPerPopulation[REPLACED]))
				if aWarWearinessModifier[BASE] > aWarWearinessModifier[REPLACED]: #iWarWearinessModifier - lower value is better, as it increases unhappiness from war
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have War Weariness "+str(aWarWearinessModifier[BASE])+"/"+str(aWarWearinessModifier[REPLACED]))
				if aGlobalWarWearinessModifier[BASE] > aGlobalWarWearinessModifier[REPLACED]: #iGlobalWarWearinessModifier - lower value is better, as it increases unhappiness from war
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global War Weariness "+str(aGlobalWarWearinessModifier[BASE])+"/"+str(aGlobalWarWearinessModifier[REPLACED]))
				if aEnemyWarWearinessModifier[BASE] < aEnemyWarWearinessModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Enemy War Weariness "+str(aEnemyWarWearinessModifier[BASE])+"/"+str(aEnemyWarWearinessModifier[REPLACED]))
				if aAllCityDefense[BASE] < aAllCityDefense[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have All City Defense "+str(aAllCityDefense[BASE])+"/"+str(aAllCityDefense[REPLACED]))
				if aBombardDefense[BASE] < aBombardDefense[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Bombard Defense "+str(aBombardDefense[BASE])+"/"+str(aBombardDefense[REPLACED]))
				if aBuildingDefenseRecoverySpeedModifier[BASE] < aBuildingDefenseRecoverySpeedModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Building Defense Recovery Speed "+str(aBuildingDefenseRecoverySpeedModifier[BASE])+"/"+str(aBuildingDefenseRecoverySpeedModifier[REPLACED]))
				if aCityDefenseRecoverySpeedModifier[BASE] < aCityDefenseRecoverySpeedModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have City Defense Recovery Speed "+str(aCityDefenseRecoverySpeedModifier[BASE])+"/"+str(aCityDefenseRecoverySpeedModifier[REPLACED]))
				if aDefense[BASE] < aDefense[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Defense "+str(aDefense[BASE])+"/"+str(aDefense[REPLACED]))
				if aEspionageDefense[BASE] < aEspionageDefense[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Espionage Defense "+str(aEspionageDefense[BASE])+"/"+str(aEspionageDefense[REPLACED]))
				if aLocalDynamicDefense[BASE] < aLocalDynamicDefense[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Local Dynamic Defense "+str(aLocalDynamicDefense[BASE])+"/"+str(aLocalDynamicDefense[REPLACED]))
				if aMinDefense[BASE] < aMinDefense[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Min Defense "+str(aMinDefense[BASE])+"/"+str(aMinDefense[REPLACED]))
				if aNoEntryDefenseLevel[BASE] > aNoEntryDefenseLevel[REPLACED] and not (aNoEntryDefenseLevel[BASE] > 0 and aNoEntryDefenseLevel[REPLACED] == 0):  #iNoEntryDefenseLevel - lower value is better, as units have to dig trough to lower level of defense, value of 0 means no need to dig trough defense.
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have No Entry Defense Level "+str(aNoEntryDefenseLevel[BASE])+"/"+str(aNoEntryDefenseLevel[REPLACED]))
				if aRiverDefensePenalty[BASE] < aRiverDefensePenalty[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have River Defense Penalty "+str(aRiverDefensePenalty[BASE])+"/"+str(aRiverDefensePenalty[REPLACED]))
				if aExperience[BASE] < aExperience[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Free Experience "+str(aExperience[BASE])+"/"+str(aExperience[REPLACED]))
				if aGlobalExperience[BASE] < aGlobalExperience[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Free global Experience "+str(aGlobalExperience[BASE])+"/"+str(aGlobalExperience[REPLACED]))
				if aFoodKept[BASE] < aFoodKept[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Food Kept "+str(aFoodKept[BASE])+"/"+str(aFoodKept[REPLACED]))
				if aPopulationgrowthratepercentage[BASE] > aPopulationgrowthratepercentage[REPLACED]: #iPopulationgrowthratepercentage - lower value is better, as it decreases food needed for next pop
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Population growth rate percentage "+str(aPopulationgrowthratepercentage[BASE])+"/"+str(aPopulationgrowthratepercentage[REPLACED]))
				if aAdjacentDamagePercent[BASE] < aAdjacentDamagePercent[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Adjacent Damage Percent "+str(aAdjacentDamagePercent[BASE])+"/"+str(aAdjacentDamagePercent[REPLACED]))
				if aLineOfSight[BASE] < aLineOfSight[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Line of Sight "+str(aLineOfSight[BASE])+"/"+str(aLineOfSight[REPLACED]))
				if aAirModifier[BASE] > aAirModifier[REPLACED]: #Lower iAirModifier is better
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Air Modifier "+str(aAirModifier[BASE])+"/"+str(aAirModifier[REPLACED]))
				if aAirUnitCapacity[BASE] < aAirUnitCapacity[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Air Unit Capacity "+str(aAirUnitCapacity[BASE])+"/"+str(aAirUnitCapacity[REPLACED]))
				if aAirlift[BASE] < aAirlift[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Airlift "+str(aAirlift[BASE])+"/"+str(aAirlift[REPLACED]))
				if aAnarchyModifier[BASE] < aAnarchyModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Anarchy Modifier "+str(aAnarchyModifier[BASE])+"/"+str(aAnarchyModifier[REPLACED]))
				if aAreaHappiness[BASE] < aAreaHappiness[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Area Happiness "+str(aAreaHappiness[BASE])+"/"+str(aAreaHappiness[REPLACED]))
				if aAreaHealth[BASE] < aAreaHealth[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Area Health "+str(aAreaHealth[BASE])+"/"+str(aAreaHealth[REPLACED]))
				if aDamageAttackerChance[BASE] < aDamageAttackerChance[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Damage Attacker Chance "+str(aDamageAttackerChance[BASE])+"/"+str(aDamageAttackerChance[REPLACED]))
				if aDamageToAttacker[BASE] < aDamageToAttacker[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Damage To Attacker "+str(aDamageToAttacker[BASE])+"/"+str(aDamageToAttacker[REPLACED]))
				if aDistanceMaintenanceModifier[BASE] < aDistanceMaintenanceModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Distance Maintenance Modifier "+str(aDistanceMaintenanceModifier[BASE])+"/"+str(aDistanceMaintenanceModifier[REPLACED]))
				if aDomesticGreatGeneralRateModifier[BASE] < aDomesticGreatGeneralRateModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Domestic Great General Rate Modifier "+str(aDomesticGreatGeneralRateModifier[BASE])+"/"+str(aDomesticGreatGeneralRateModifier[REPLACED]))
				if aGlobalGreatPeopleRateModifier[BASE] < aGlobalGreatPeopleRateModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global Great People Rate Modifier "+str(aGlobalGreatPeopleRateModifier[BASE])+"/"+str(aGlobalGreatPeopleRateModifier[REPLACED]))
				if aGlobalHappiness[BASE] < aGlobalHappiness[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global Happiness "+str(aGlobalHappiness[BASE])+"/"+str(aGlobalHappiness[REPLACED]))
				if aGlobalHealth[BASE] < aGlobalHealth[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global Health "+str(aGlobalHealth[BASE])+"/"+str(aGlobalHealth[REPLACED]))
				if aGlobalMaintenanceModifier[BASE] < aGlobalMaintenanceModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global Maintenance Modifier "+str(aGlobalMaintenanceModifier[BASE])+"/"+str(aGlobalMaintenanceModifier[REPLACED]))
				if aGlobalSpaceProductionModifier[BASE] < aGlobalSpaceProductionModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global Space Production Modifier "+str(aGlobalSpaceProductionModifier[BASE])+"/"+str(aGlobalSpaceProductionModifier[REPLACED]))
				if aGoldenAgeModifier[BASE] < aGoldenAgeModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Golden Age Modifier "+str(aGoldenAgeModifier[BASE])+"/"+str(aGoldenAgeModifier[REPLACED]))
				if aGreatGeneralRateModifier[BASE] < aGreatGeneralRateModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Great General Rate Modifier "+str(aGreatGeneralRateModifier[BASE])+"/"+str(aGreatGeneralRateModifier[REPLACED]))
				if aHealRateChange[BASE] < aHealRateChange[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Heal Rate Change "+str(aHealRateChange[BASE])+"/"+str(aHealRateChange[REPLACED]))
				if aInsidiousness[BASE] > aInsidiousness[REPLACED]: #Lower iInsidiousness is better
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Insidiousness "+str(aInsidiousness[BASE])+"/"+str(aInsidiousness[REPLACED]))
				if aInvasionChance[BASE] > aInvasionChance[REPLACED]: #Lower iInvasionChance is better
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Invasion Chance "+str(aInvasionChance[BASE])+"/"+str(aInvasionChance[REPLACED]))
				if aInvestigation[BASE] < aInvestigation[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Investigation "+str(aInvestigation[BASE])+"/"+str(aInvestigation[REPLACED]))
				if aLocalRepel[BASE] < aLocalRepel[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Local Repel "+str(aLocalRepel[BASE])+"/"+str(aLocalRepel[REPLACED]))
				if aMilitaryProductionModifier[BASE] < aMilitaryProductionModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Military Production Modifier "+str(aMilitaryProductionModifier[BASE])+"/"+str(aMilitaryProductionModifier[REPLACED]))
				if aNationalCaptureProbabilityModifier[BASE] < aNationalCaptureProbabilityModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have National Capture Probability Modifier "+str(aNationalCaptureProbabilityModifier[BASE])+"/"+str(aNationalCaptureProbabilityModifier[REPLACED]))
				if aNationalCaptureResistanceModifier[BASE] < aNationalCaptureResistanceModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have National Capture Resistance Modifier "+str(aNationalCaptureResistanceModifier[BASE])+"/"+str(aNationalCaptureResistanceModifier[REPLACED]))
				if aNukeModifier[BASE] > aNukeModifier[REPLACED]: #Lower iNukeModifier is better
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Nuke Modifier "+str(aNukeModifier[BASE])+"/"+str(aNukeModifier[REPLACED]))
				if aNumUnitFullHeal[BASE] < aNumUnitFullHeal[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Num Unit Full Heal "+str(aNumUnitFullHeal[BASE])+"/"+str(aNumUnitFullHeal[REPLACED]))
				if aRevIdxLocal[BASE] > aRevIdxLocal[REPLACED]: #Lower iRevIdxLocal is better
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have RevIdx Local "+str(aRevIdxLocal[BASE])+"/"+str(aRevIdxLocal[REPLACED]))
				if aRevIdxNational[BASE] > aRevIdxNational[REPLACED]: #Lower iRevIdxNational is better
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have RevIdx National "+str(aRevIdxNational[BASE])+"/"+str(aRevIdxNational[REPLACED]))
				if aSpaceProductionModifier[BASE] < aSpaceProductionModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Space Production Modifier "+str(aSpaceProductionModifier[BASE])+"/"+str(aSpaceProductionModifier[REPLACED]))
				if aStateReligionHappiness[BASE] < aStateReligionHappiness[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have State Religion Happiness "+str(aStateReligionHappiness[BASE])+"/"+str(aStateReligionHappiness[REPLACED]))
				if aUnitUpgradePriceModifier[BASE] < aUnitUpgradePriceModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Unit Upgrade Price Modifier "+str(aUnitUpgradePriceModifier[BASE])+"/"+str(aUnitUpgradePriceModifier[REPLACED]))
				if aWorkerSpeedModifier[BASE] < aWorkerSpeedModifier[REPLACED]:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Worker Speed Modifier "+str(aWorkerSpeedModifier[BASE])+"/"+str(aWorkerSpeedModifier[REPLACED]))

				#===== 1D ENTRIES - ARRAYS, index of array is an infotype ENUM =================================================================#
				#<YieldChanges>, <YieldPerPopChanges>, <RiverPlotYieldChanges>, <YieldModifiers>, <PowerYieldModifiers>, <AreaYieldModifiers>, <GlobalYieldModifiers>, <GlobalSeaPlotYieldChanges> - base
				aYieldChangesList = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aYieldPerPopChangesList = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aRiverPlotYieldChangesList = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aYieldModifiersList = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aPowerYieldModifiersList = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aAreaYieldModifiersList = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aGlobalYieldModifiersList = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aGlobalSeaPlotYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
					aYieldChangesList[BASE][iYield] += CvBuildingInfo.getYieldChange(iYield)
					aYieldPerPopChangesList[BASE][iYield] += CvBuildingInfo.getYieldPerPopChange(iYield)
					aRiverPlotYieldChangesList[BASE][iYield] += CvBuildingInfo.getRiverPlotYieldChange(iYield)
					aYieldModifiersList[BASE][iYield] += CvBuildingInfo.getYieldModifier(iYield)
					aPowerYieldModifiersList[BASE][iYield] += CvBuildingInfo.getPowerYieldModifier(iYield)
					aAreaYieldModifiersList[BASE][iYield] += CvBuildingInfo.getAreaYieldModifier(iYield)
					aGlobalYieldModifiersList[BASE][iYield] += CvBuildingInfo.getGlobalYieldModifier(iYield)
					aGlobalSeaPlotYieldChanges[BASE][iYield] += CvBuildingInfo.getGlobalSeaPlotYieldChange(iYield)

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<YieldChanges>, <YieldPerPopChanges>, <RiverPlotYieldChanges>, <YieldModifiers>, <PowerYieldModifiers>, <AreaYieldModifiers>, <GlobalYieldModifiers>, <GlobalSeaPlotYieldChanges>
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						aYieldChangesList[REPLACED][iYield] += CvReplacedBuildingInfo.getYieldChange(iYield)
						aYieldPerPopChangesList[REPLACED][iYield] += CvReplacedBuildingInfo.getYieldPerPopChange(iYield)
						aRiverPlotYieldChangesList[REPLACED][iYield] += CvReplacedBuildingInfo.getRiverPlotYieldChange(iYield)
						aYieldModifiersList[REPLACED][iYield] += CvReplacedBuildingInfo.getYieldModifier(iYield)
						aPowerYieldModifiersList[REPLACED][iYield] += CvReplacedBuildingInfo.getPowerYieldModifier(iYield)
						aAreaYieldModifiersList[REPLACED][iYield] += CvReplacedBuildingInfo.getAreaYieldModifier(iYield)
						aGlobalYieldModifiersList[REPLACED][iYield] += CvReplacedBuildingInfo.getGlobalYieldModifier(iYield)
						aGlobalSeaPlotYieldChanges[REPLACED][iYield] += CvReplacedBuildingInfo.getGlobalSeaPlotYieldChange(iYield)

				#Building shouldn't be worse than replaced one!
				for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
					if aYieldChangesList[BASE][iYield] < aYieldChangesList[REPLACED][iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" Yield Changes "+str(aYieldChangesList[BASE])+"/"+str(aYieldChangesList[REPLACED]))
					if aYieldPerPopChangesList[BASE][iYield] < aYieldPerPopChangesList[REPLACED][iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" Yield Per pop Changes "+str(aYieldPerPopChangesList[BASE])+"/"+str(aYieldPerPopChangesList[REPLACED]))
					if aRiverPlotYieldChangesList[BASE][iYield] < aRiverPlotYieldChangesList[REPLACED][iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" River plot Yield Changes "+str(aRiverPlotYieldChangesList[BASE])+"/"+str(aRiverPlotYieldChangesList[REPLACED]))
					if aYieldModifiersList[BASE][iYield] < aYieldModifiersList[REPLACED][iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" Yield Modifiers "+str(aYieldModifiersList[BASE])+"/"+str(aYieldModifiersList[REPLACED]))
					if aPowerYieldModifiersList[BASE][iYield] < aPowerYieldModifiersList[REPLACED][iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" Yield Power Modifiers "+str(aPowerYieldModifiersList[BASE])+"/"+str(aPowerYieldModifiersList[REPLACED]))
					if aAreaYieldModifiersList[BASE][iYield] < aAreaYieldModifiersList[REPLACED][iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" Yield Area Modifiers "+str(aAreaYieldModifiersList[BASE])+"/"+str(aAreaYieldModifiersList[REPLACED]))
					if aGlobalYieldModifiersList[BASE][iYield] < aGlobalYieldModifiersList[REPLACED][iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" Yield Global Modifiers "+str(aGlobalYieldModifiersList[BASE])+"/"+str(aGlobalYieldModifiersList[REPLACED]))
					if aGlobalSeaPlotYieldChanges[BASE][iYield] < aGlobalSeaPlotYieldChanges[REPLACED][iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" Global sea plot Yield Changes "+str(aGlobalSeaPlotYieldChanges[BASE])+"/"+str(aGlobalSeaPlotYieldChanges[REPLACED]))

				#=================================================================================================
				#<CommerceChanges>, <CommercePerPopChanges>, <CommerceModifiers>, <GlobalCommerceModifiers>, <CommerceHappinesses>, <SpecialistExtraCommerces> - base
				aCommerceChanges = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aCommercePerPopChanges = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aCommerceModifiers = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aGlobalCommerceModifiers = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aCommerceHappinesses = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aSpecialistExtraCommerces = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
					aCommerceChanges[BASE][iCommerce] += CvBuildingInfo.getCommerceChange(iCommerce)
					aCommercePerPopChanges[BASE][iCommerce] += CvBuildingInfo.getCommercePerPopChange(iCommerce)
					aCommerceModifiers[BASE][iCommerce] += CvBuildingInfo.getCommerceModifier(iCommerce)
					aGlobalCommerceModifiers[BASE][iCommerce] += CvBuildingInfo.getGlobalCommerceModifier(iCommerce)
					aCommerceHappinesses[BASE][iCommerce] += CvBuildingInfo.getCommerceHappiness(iCommerce)
					aSpecialistExtraCommerces[BASE][iCommerce] += CvBuildingInfo.getSpecialistExtraCommerce(iCommerce)

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<CommerceChanges>, <CommercePerPopChanges>, <CommerceModifiers>, <GlobalCommerceModifiers>, <CommerceHappinesses>, <SpecialistExtraCommerces>
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						aCommerceChanges[REPLACED][iCommerce] += CvReplacedBuildingInfo.getCommerceChange(iCommerce)
						aCommercePerPopChanges[REPLACED][iCommerce] += CvReplacedBuildingInfo.getCommercePerPopChange(iCommerce)
						aCommerceModifiers[REPLACED][iCommerce] += CvReplacedBuildingInfo.getCommerceModifier(iCommerce)
						aGlobalCommerceModifiers[REPLACED][iCommerce] += CvReplacedBuildingInfo.getGlobalCommerceModifier(iCommerce)
						aCommerceHappinesses[REPLACED][iCommerce] += CvReplacedBuildingInfo.getCommerceHappiness(iCommerce)
						aSpecialistExtraCommerces[REPLACED][iCommerce] += CvReplacedBuildingInfo.getSpecialistExtraCommerce(iCommerce)

				#Building shouldn't be worse than replaced one!
				for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
					if aCommerceChanges[BASE][iCommerce] < aCommerceChanges[REPLACED][iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getCommerceInfo(iCommerce).getType()+" Commerce Changes "+str(aCommerceChanges[BASE])+"/"+str(aCommerceChanges[REPLACED]))
					if aCommercePerPopChanges[BASE][iCommerce] < aCommercePerPopChanges[REPLACED][iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getCommerceInfo(iCommerce).getType()+" Commerce Per pop Changes "+str(aCommercePerPopChanges[BASE])+"/"+str(aCommercePerPopChanges[REPLACED]))
					if aCommerceModifiers[BASE][iCommerce] < aCommerceModifiers[REPLACED][iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getCommerceInfo(iCommerce).getType()+" Commerce Modifiers "+str(aCommerceModifiers[BASE])+"/"+str(aCommerceModifiers[REPLACED]))
					if aGlobalCommerceModifiers[BASE][iCommerce] < aGlobalCommerceModifiers[REPLACED][iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getCommerceInfo(iCommerce).getType()+" Commerce Global Modifiers "+str(aGlobalCommerceModifiers[BASE])+"/"+str(aGlobalCommerceModifiers[REPLACED]))
					if aCommerceHappinesses[BASE][iCommerce] < aCommerceHappinesses[REPLACED][iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getCommerceInfo(iCommerce).getType()+" Commerce Happinesses "+str(aCommerceHappinesses[BASE])+"/"+str(aCommerceHappinesses[REPLACED]))
					if aSpecialistExtraCommerces[BASE][iCommerce] < aSpecialistExtraCommerces[REPLACED][iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getCommerceInfo(iCommerce).getType()+" Commerce Specialist extra Commerces "+str(aSpecialistExtraCommerces[BASE])+"/"+str(aSpecialistExtraCommerces[REPLACED]))

				#=================================================================================================
				#<SpecialistCounts>, <FreeSpecialistCounts> - base
				aSpecialistCounts = [[0 for x in xrange(GC.getNumSpecialistInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aFreeSpecialistCounts = [[0 for x in xrange(GC.getNumSpecialistInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				for iSpecialist in xrange(GC.getNumSpecialistInfos()):
					aSpecialistCounts[BASE][iSpecialist] += CvBuildingInfo.getSpecialistCount(iSpecialist)
					aFreeSpecialistCounts[BASE][iSpecialist] += CvBuildingInfo.getFreeSpecialistCount(iSpecialist)

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<SpecialistCounts>, <FreeSpecialistCounts>
					for iSpecialist in xrange(GC.getNumSpecialistInfos()):
						aSpecialistCounts[REPLACED][iSpecialist] += CvReplacedBuildingInfo.getSpecialistCount(iSpecialist)
						aFreeSpecialistCounts[REPLACED][iSpecialist] += CvReplacedBuildingInfo.getFreeSpecialistCount(iSpecialist)

				#Building shouldn't be worse than replaced one!
				#Emancipation Proclamation removes worldview buildings, python is needed for it to actually erase all slave specialists
				for iSpecialist in xrange(GC.getNumSpecialistInfos()):
					if aSpecialistCounts[BASE][iSpecialist] < aSpecialistCounts[REPLACED][iSpecialist] and iBuilding != GC.getInfoTypeForString("BUILDING_EMANCIPATION_PROCLAMATION_EFFECT"):
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getSpecialistInfo(iSpecialist).getType()+" Specialists Count "+str(aSpecialistCounts[BASE][iSpecialist])+"/"+str(aSpecialistCounts[REPLACED][iSpecialist]))
					if aFreeSpecialistCounts[BASE][iSpecialist] < aFreeSpecialistCounts[REPLACED][iSpecialist] and iBuilding != GC.getInfoTypeForString("BUILDING_EMANCIPATION_PROCLAMATION_EFFECT"):
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getSpecialistInfo(iSpecialist).getType()+" Free specialists Count "+str(aFreeSpecialistCounts[BASE][iSpecialist])+"/"+str(aFreeSpecialistCounts[REPLACED][iSpecialist]))

				#=================================================================================================
				#<ImprovementFreeSpecialists> - base
				aImprovementFreeSpecialists = [[0 for x in xrange(GC.getNumImprovementInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				for iImprovement, iNumFreeSpecialists in CvBuildingInfo.getImprovementFreeSpecialists():
					aImprovementFreeSpecialists[BASE][iImprovement] += iNumFreeSpecialists

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<ImprovementFreeSpecialists>
					for iImprovement, iNumFreeSpecialists in CvReplacedBuildingInfo.getImprovementFreeSpecialists():
						aImprovementFreeSpecialists[REPLACED][iImprovement] += iNumFreeSpecialists

				#Building shouldn't be worse than replaced one!
				for iImprovement in xrange(GC.getNumImprovementInfos()):
					if aImprovementFreeSpecialists[BASE][iImprovement] < aImprovementFreeSpecialists[REPLACED][iImprovement]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getImprovementInfo(iImprovement).getType()+" Improvement free Specialist "+str(aImprovementFreeSpecialists[BASE][iImprovement])+"/"+str(aImprovementFreeSpecialists[REPLACED][iImprovement]))

				#=================================================================================================
				#<BonusHappinessChanges>, <BonusHealthChanges>, <ExtraFreeBonuses> - base
				aBonusHappinessChanges = [[0 for x in xrange(GC.getNumBonusInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aBonusHealthChanges = [[0 for x in xrange(GC.getNumBonusInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aExtraFreeBonuses = [[0 for x in xrange(GC.getNumBonusInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				for iBonus, iHappiness in CvBuildingInfo.getBonusHappinessChanges():
					aBonusHappinessChanges[BASE][iBonus] += iHappiness
				for iBonus, iHealth in CvBuildingInfo.getBonusHealthChanges():
					aBonusHealthChanges[BASE][iBonus] += iHealth
				for iBonus, iNumFree in CvBuildingInfo.getFreeBonuses():
					aExtraFreeBonuses[BASE][iBonus] += iNumFree

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<BonusHappinessChanges>, <BonusHealthChanges>, <ExtraFreeBonuses>
					for iBonus, iHappiness in CvReplacedBuildingInfo.getBonusHappinessChanges():
						aBonusHappinessChanges[REPLACED][iBonus] += iHappiness
					for iBonus, iHealth in CvReplacedBuildingInfo.getBonusHealthChanges():
						aBonusHealthChanges[REPLACED][iBonus] += iHealth
					for iBonus, iNumFree in CvReplacedBuildingInfo.getFreeBonuses():
						aExtraFreeBonuses[REPLACED][iBonus] += iNumFree

				#Building shouldn't be worse than replaced one!
				for iBonus in xrange(GC.getNumBonusInfos()):
					if aBonusHappinessChanges[BASE][iBonus] < aBonusHappinessChanges[REPLACED][iBonus]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" Bonus happiness Changes "+str(aBonusHappinessChanges[BASE][iBonus])+"/"+str(aBonusHappinessChanges[REPLACED][iBonus]))
					if aBonusHealthChanges[BASE][iBonus] < aBonusHealthChanges[REPLACED][iBonus]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" Bonus health Changes "+str(aBonusHealthChanges[BASE][iBonus])+"/"+str(aBonusHealthChanges[REPLACED][iBonus]))
					if iBonus not in aCorporateBonusList and aExtraFreeBonuses[BASE][iBonus] == 0 and aExtraFreeBonuses[REPLACED][iBonus] > 0: #No corporation using it - check presence only
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should give "+GC.getBonusInfo(iBonus).getType()+" Bonus")
					if iBonus in aCorporateBonusList and aExtraFreeBonuses[BASE][iBonus] < aExtraFreeBonuses[REPLACED][iBonus]: #Bonus used by corporations, so add up them
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should give "+GC.getBonusInfo(iBonus).getType()+" Bonus (for use by corporations) "+str(aExtraFreeBonuses[BASE][iBonus])+"/"+str(aExtraFreeBonuses[REPLACED][iBonus]))

				#=================================================================================================
				#<TechHappinessChanges>, <TechHealthChanges> - base
				aTechHappinessChanges = [[0 for x in xrange(GC.getNumTechInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aTechHealthChanges = [[0 for x in xrange(GC.getNumTechInfos())] for y in xrange(MAIN_ARRAY_SIZE)]

				iEarlyHappinessChanges = 0
				iEarlyHealthChanges = 0
				aEarlyHappinessTechs = []
				aEarlyHealthTechs = []

				for iTech, iHappiness in CvBuildingInfo.getTechHappinessChanges():
					if GC.getTechInfo(iTech).getGridX() > iTechBase:
						aTechHappinessChanges[BASE][iTech] += iHappiness
					else:
						aEarlyHappinessTechs.append(GC.getTechInfo(iTech).getType())
				for iTech, iHealth in CvBuildingInfo.getTechHealthChanges():
					if GC.getTechInfo(iTech).getGridX() > iTechBase:
						aTechHealthChanges[BASE][iTech] += iHealth
					else:
						aEarlyHealthTechs.append(GC.getTechInfo(iTech).getType())

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<TechHappinessChanges>, <TechHealthChanges>
					for iTech, iHappiness in CvReplacedBuildingInfo.getTechHappinessChanges():
						if GC.getTechInfo(iTech).getGridX() > iTechBase:
							aTechHappinessChanges[REPLACED][iTech] += iHappiness
						else:
							iEarlyHappinessChanges += iHappiness
					for iTech, iHealth in CvReplacedBuildingInfo.getTechHealthChanges():
						if GC.getTechInfo(iTech).getGridX() > iTechBase:
							aTechHealthChanges[REPLACED][iTech] += iHealth
						else:
							iEarlyHealthChanges += iHealth

				#Building shouldn't be worse than replaced one!
				for iTech in xrange(GC.getNumTechInfos()):
					if GC.getTechInfo(iTech).getGridX() > iTechBase:
						if aTechHappinessChanges[BASE][iTech] < aTechHappinessChanges[REPLACED][iTech]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech happiness Changes "+str(aTechHappinessChanges[BASE][iTech])+"/"+str(aTechHappinessChanges[REPLACED][iTech]))
						if aTechHealthChanges[BASE][iTech] < aTechHealthChanges[REPLACED][iTech]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech health Changes "+str(aTechHealthChanges[BASE][iTech])+"/"+str(aTechHealthChanges[REPLACED][iTech]))

				if aHappiness[BASE] + iEarlyHappinessChanges > aHappiness[REPLACED] and len(aEarlyHappinessTechs) > 0:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Early Happiness Changes: "+str(aHappiness[BASE] + iEarlyHappinessChanges)+" Early Happiness Changes Techs: "+str(aEarlyHappinessTechs))
				if aHealth[BASE] + iEarlyHealthChanges > aHealth[REPLACED] and len(aEarlyHealthTechs) > 0:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Early Health Changes: "+str(aHealth[BASE] + iEarlyHealthChanges)+" Early Health Changes Techs: "+str(aEarlyHealthTechs))

				#=============================================================================================================================
				#<GlobalBuildingCostModifiers>, <GlobalBuildingProductionModifiers>, <BuildingHappinessChanges>, <BuildingProductionModifiers> - base
				aGlobalBuildingCostModifiers = [[0 for x in xrange(GC.getNumBuildingInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aGlobalBuildingProductionModifiers = [[0 for x in xrange(GC.getNumBuildingInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aBuildingHappinessChanges = [[0 for x in xrange(GC.getNumBuildingInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aBuildingProductionModifiers = [[0 for x in xrange(GC.getNumBuildingInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				for iBuilding, iCostModifier in CvBuildingInfo.getGlobalBuildingCostModifiers():
					aGlobalBuildingCostModifiers[BASE][iBuilding] += iCostModifier
				for iBuilding, iGlobalProductionModifier in CvBuildingInfo.getGlobalBuildingProductionModifiers():
					aGlobalBuildingProductionModifiers[BASE][iBuilding] += iGlobalProductionModifier
				for iBuilding, iHappiness in CvBuildingInfo.getBuildingHappinessChanges():
					aBuildingHappinessChanges[BASE][iBuilding] += iHappiness
				for iBuilding, iProductionModifier in CvBuildingInfo.getBuildingProductionModifiers():
					aBuildingProductionModifiers[BASE][iBuilding] += iProductionModifier

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<GlobalBuildingCostModifiers>, <GlobalBuildingProductionModifiers>, <BuildingHappinessChanges>, <BuildingProductionModifiers>
					for iBuilding, iCostModifier in CvReplacedBuildingInfo.getGlobalBuildingCostModifiers():
						aGlobalBuildingCostModifiers[REPLACED][iBuilding] += iCostModifier
					for iBuilding, iGlobalProductionModifier in CvReplacedBuildingInfo.getGlobalBuildingProductionModifiers():
						aGlobalBuildingProductionModifiers[REPLACED][iBuilding] += iGlobalProductionModifier
					for iBuilding, iHappiness in CvReplacedBuildingInfo.getBuildingHappinessChanges():
						aBuildingHappinessChanges[REPLACED][iBuilding] += iHappiness
					for iBuilding, iProductionModifier in CvReplacedBuildingInfo.getBuildingProductionModifiers():
						aBuildingProductionModifiers[REPLACED][iBuilding] += iProductionModifier

				#Building shouldn't be worse than replaced one!
				for iAffectedBuildings in xrange(GC.getNumBuildingInfos()):
					if aGlobalBuildingCostModifiers[BASE][iAffectedBuildings] < aGlobalBuildingCostModifiers[REPLACED][iAffectedBuildings]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuildings).getType()+" Global Building cost Modifiers "+str(aGlobalBuildingCostModifiers[BASE][iAffectedBuildings])+"/"+str(aGlobalBuildingCostModifiers[REPLACED][iAffectedBuildings]))
					if aGlobalBuildingProductionModifiers[BASE][iAffectedBuildings] < aGlobalBuildingProductionModifiers[REPLACED][iAffectedBuildings]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuildings).getType()+" Global Building production Modifiers "+str(aGlobalBuildingProductionModifiers[BASE][iAffectedBuildings])+"/"+str(aGlobalBuildingProductionModifiers[REPLACED][iAffectedBuildings]))
					if aBuildingHappinessChanges[BASE][iAffectedBuildings] < aBuildingHappinessChanges[REPLACED][iAffectedBuildings]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuildings).getType()+" Building Happiness Changes "+str(aBuildingHappinessChanges[BASE][iAffectedBuildings])+"/"+str(aBuildingHappinessChanges[REPLACED][iAffectedBuildings]))
					if aBuildingProductionModifiers[BASE][iAffectedBuildings] < aBuildingProductionModifiers[REPLACED][iAffectedBuildings]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuildings).getType()+" Building production Modifiers "+str(aBuildingProductionModifiers[BASE][iAffectedBuildings])+"/"+str(aBuildingProductionModifiers[REPLACED][iAffectedBuildings]))

				#=================================================================================================
				#<DomainFreeExperiences>, <DomainProductionModifiers> - base
				aDomainFreeExperiences = [[0 for x in xrange(DomainTypes.NUM_DOMAIN_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				aDomainProductionModifiers = [[0 for x in xrange(DomainTypes.NUM_DOMAIN_TYPES)] for y in xrange(MAIN_ARRAY_SIZE)]
				for iDomain in xrange(DomainTypes.NUM_DOMAIN_TYPES):
					aDomainFreeExperiences[BASE][iDomain] += CvBuildingInfo.getDomainFreeExperience(iDomain)
					aDomainProductionModifiers[BASE][iDomain] += CvBuildingInfo.getDomainProductionModifier(iDomain)

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<DomainFreeExperiences>, <DomainProductionModifiers>
					for iDomain in xrange(DomainTypes.NUM_DOMAIN_TYPES):
						aDomainFreeExperiences[REPLACED][iDomain] += CvReplacedBuildingInfo.getDomainFreeExperience(iDomain)
						aDomainProductionModifiers[REPLACED][iDomain] += CvReplacedBuildingInfo.getDomainProductionModifier(iDomain)

				#Building shouldn't be worse than replaced one!
				for iDomain in xrange(DomainTypes.NUM_DOMAIN_TYPES):
					if aDomainFreeExperiences[BASE][iDomain] < aDomainFreeExperiences[REPLACED][iDomain]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getDomainInfo(iDomain).getType()+" Domain Free Experience "+str(aDomainFreeExperiences[BASE][iDomain])+"/"+str(aDomainFreeExperiences[REPLACED][iDomain]))
					if aDomainProductionModifiers[BASE][iDomain] < aDomainProductionModifiers[REPLACED][iDomain]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getDomainInfo(iDomain).getType()+" Domain production Modifiers "+str(aDomainProductionModifiers[BASE][iDomain])+"/"+str(aDomainProductionModifiers[REPLACED][iDomain]))

				#=================================================================================================
				#<UnitCombatFreeExperiences>, <HealUnitCombatTypes>, <UnitCombatExtraStrengths>, <UnitCombatProdModifiers> - base
				aUnitCombatFreeExperiences = [[0 for x in xrange(GC.getNumUnitCombatInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aHealUnitCombatTypes = [[0 for x in xrange(GC.getNumUnitCombatInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aUnitCombatExtraStrengths = [[0 for x in xrange(GC.getNumUnitCombatInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aUnitCombatProdModifiers = [[0 for x in xrange(GC.getNumUnitCombatInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				for iUnitCombat, iFreeExperience in CvBuildingInfo.getUnitCombatFreeExperience():
					aUnitCombatFreeExperiences[BASE][iUnitCombat] += iFreeExperience
				for iUnitCombat, iExtraStrength in CvBuildingInfo.getUnitCombatExtraStrength():
					aUnitCombatExtraStrengths[BASE][iUnitCombat] += iExtraStrength
				for i in xrange(CvBuildingInfo.getNumHealUnitCombatTypes()):
					iUnitCombat = CvBuildingInfo.getHealUnitCombatType(i).eUnitCombat
					aHealUnitCombatTypes[BASE][iUnitCombat] += CvBuildingInfo.getHealUnitCombatType(i).value
				for iUnitCombat in xrange(GC.getNumUnitCombatInfos()):
					aUnitCombatProdModifiers[BASE][iUnitCombat] += CvBuildingInfo.getUnitCombatProdModifier(iUnitCombat)

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<UnitCombatFreeExperiences>, <HealUnitCombatTypes>, <UnitCombatExtraStrengths>, <UnitCombatProdModifiers>
					for iUnitCombat, iFreeExperience in CvReplacedBuildingInfo.getUnitCombatFreeExperience():
						aUnitCombatFreeExperiences[REPLACED][iUnitCombat] += iFreeExperience
					for iUnitCombat, iExtraStrength in CvReplacedBuildingInfo.getUnitCombatExtraStrength():
						aUnitCombatExtraStrengths[REPLACED][iUnitCombat] += iExtraStrength
					for j in xrange(CvReplacedBuildingInfo.getNumHealUnitCombatTypes()):
						iUnitCombat = CvReplacedBuildingInfo.getHealUnitCombatType(j).eUnitCombat
						aHealUnitCombatTypes[REPLACED][iUnitCombat] += CvReplacedBuildingInfo.getHealUnitCombatType(j).value
					for iUnitCombat in xrange(GC.getNumUnitCombatInfos()):
						aUnitCombatProdModifiers[REPLACED][iUnitCombat] += CvReplacedBuildingInfo.getUnitCombatProdModifier(iUnitCombat)

				#Building shouldn't be worse than replaced one!
				for iUnitCombat in xrange(GC.getNumUnitCombatInfos()):
					if aUnitCombatFreeExperiences[BASE][iUnitCombat] < aUnitCombatFreeExperiences[REPLACED][iUnitCombat]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getUnitCombatInfo(iUnitCombat).getType()+" Unit Combat free Experience "+str(aUnitCombatFreeExperiences[BASE][iUnitCombat])+"/"+str(aUnitCombatFreeExperiences[REPLACED][iUnitCombat]))
					if aHealUnitCombatTypes[BASE][iUnitCombat] < aHealUnitCombatTypes[REPLACED][iUnitCombat]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getUnitCombatInfo(iUnitCombat).getType()+" Heal Unit Combat "+str(aHealUnitCombatTypes[BASE][iUnitCombat])+"/"+str(aHealUnitCombatTypes[REPLACED][iUnitCombat]))
					if aUnitCombatExtraStrengths[BASE][iUnitCombat] < aUnitCombatExtraStrengths[REPLACED][iUnitCombat]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getUnitCombatInfo(iUnitCombat).getType()+" Unit Combat extra strengths "+str(aUnitCombatExtraStrengths[BASE][iUnitCombat])+"/"+str(aUnitCombatExtraStrengths[REPLACED][iUnitCombat]))
					if aUnitCombatProdModifiers[BASE][iUnitCombat] < aUnitCombatProdModifiers[REPLACED][iUnitCombat]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getUnitCombatInfo(iUnitCombat).getType()+" Unit Combat prod Modifiers "+str(aUnitCombatProdModifiers[BASE][iUnitCombat])+"/"+str(aUnitCombatProdModifiers[REPLACED][iUnitCombat]))

				#=================================================================================================
				#<UnitProductionModifiers> - base
				aUnitProductionModifiers = [[0 for x in xrange(GC.getNumUnitInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				for iUnit, iProductionModifier in CvBuildingInfo.getUnitProductionModifiers():
					aUnitProductionModifiers[BASE][iUnit] += iProductionModifier

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<UnitProductionModifiers>
					for iUnit, iProductionModifier in CvReplacedBuildingInfo.getUnitProductionModifiers():
						aUnitProductionModifiers[REPLACED][iUnit] += iProductionModifier

				#Building shouldn't be worse than replaced one!
				for iUnit in xrange(GC.getNumUnitInfos()):
					if aUnitProductionModifiers[BASE][iUnit] < aUnitProductionModifiers[REPLACED][iUnit]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getUnitInfo(iUnit).getType()+" Unit production Modifiers "+str(aUnitProductionModifiers[BASE][iUnit])+"/"+str(aUnitProductionModifiers[REPLACED][iUnit]))

				#=================================================================================================
				#<FreePromoTypes> - base
				aFreePromoTypes = [[0 for x in xrange(GC.getNumPromotionInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				for pFreePromotionTypes in CvBuildingInfo.getFreePromoTypes():
					iPromotion = pFreePromotionTypes.ePromotion
					aFreePromoTypes[BASE][iPromotion] += 1

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<FreePromoTypes>
					for pFreePromotionTypes in CvReplacedBuildingInfo.getFreePromoTypes():
						iPromotion = pFreePromotionTypes.ePromotion
						aFreePromoTypes[REPLACED][iPromotion] += 1

				#Building shouldn't be worse than replaced one!
				for iPromotion in xrange(GC.getNumPromotionInfos()):
					if aFreePromoTypes[BASE][iPromotion] == 0 and aFreePromoTypes[REPLACED][iPromotion] != 0: #We are checking presence, as it doesn't have value
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getPromotionInfo(iPromotion).getType()+" Free Promotion")

				#=================================================================================================
				#<PropertyManipulators> - base
				aGoodProperties = [GC.getInfoTypeForString("PROPERTY_EDUCATION"), GC.getInfoTypeForString("PROPERTY_TOURISM")]
				aPropertyManipulators = [[0 for x in xrange(GC.getNumPropertyInfos())] for y in xrange(MAIN_ARRAY_SIZE)]
				aProperties = self.HF.getPropertyAmmountPerTurn(CvBuildingInfo.getPropertyManipulators())
				for iProperty in xrange(GC.getNumPropertyInfos()):
					aPropertyManipulators[BASE][iProperty] += aProperties[iProperty]

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<PropertyManipulators>
					aProperties = self.HF.getPropertyAmmountPerTurn(CvReplacedBuildingInfo.getPropertyManipulators())
					for iProperty in xrange(GC.getNumPropertyInfos()):
						aPropertyManipulators[REPLACED][iProperty] += aProperties[iProperty]

				#Building shouldn't be worse than replaced one!
				for iProperty in xrange(GC.getNumPropertyInfos()):
					if iProperty in aGoodProperties and aPropertyManipulators[BASE][iProperty] < aPropertyManipulators[REPLACED][iProperty]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getPropertyInfo(iProperty).getType()+" PropertyManipulators "+str(aPropertyManipulators[BASE][iProperty])+"/"+str(aPropertyManipulators[REPLACED][iProperty]))
					if iProperty not in aGoodProperties and aPropertyManipulators[BASE][iProperty] > aPropertyManipulators[REPLACED][iProperty]: #Bad properties - lower on replacements
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getPropertyInfo(iProperty).getType()+" PropertyManipulators "+str(aPropertyManipulators[BASE][iProperty])+"/"+str(aPropertyManipulators[REPLACED][iProperty]))

				#===== 2D ENTRIES - coupling between two infotypes, like yield changing in presence of bonus =====#
				#<BonusYieldChanges>, <VicinityBonusYieldChanges>, <BonusYieldModifiers>, <BonusCommercePercentChanges>, <BonusCommerceModifiers> - base
				aBonusYieldChanges = [[[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aVicinityBonusYieldChanges = [[[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aBonusYieldModifiers = [[[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aBonusCommercePercentChanges = [[[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBonusInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aBonusCommerceModifiers = [[[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBonusInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				if CvBuildingInfo.isAnyBonusYieldChanges():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							aBonusYieldChanges[BASE][iBonus][iYield] += CvBuildingInfo.getBonusYieldChanges(iBonus, iYield)
				if CvBuildingInfo.isAnyVicinityBonusYieldChanges():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							aVicinityBonusYieldChanges[BASE][iBonus][iYield] += CvBuildingInfo.getVicinityBonusYieldChanges(iBonus, iYield)
				if CvBuildingInfo.isAnyBonusYieldModifiers():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							aBonusYieldModifiers[BASE][iBonus][iYield] += CvBuildingInfo.getBonusYieldModifier(iBonus, iYield)
				if CvBuildingInfo.isAnyBonusCommercePercentChanges():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							aBonusCommercePercentChanges[BASE][iBonus][iCommerce] += CvBuildingInfo.getBonusCommercePercentChanges(iBonus, iCommerce)
				if CvBuildingInfo.isAnyBonusCommerceModifiers():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							aBonusCommerceModifiers[BASE][iBonus][iCommerce] += CvBuildingInfo.getBonusCommerceModifier(iBonus, iCommerce)

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<BonusYieldChanges>, <VicinityBonusYieldChanges>, <BonusYieldModifiers>, <BonusCommercePercentChanges>, <BonusCommerceModifiers>
					if CvReplacedBuildingInfo.isAnyBonusYieldChanges():
						for iBonus in xrange(GC.getNumBonusInfos()):
							for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
								aBonusYieldChanges[REPLACED][iBonus][iYield] += CvReplacedBuildingInfo.getBonusYieldChanges(iBonus, iYield)
					if CvReplacedBuildingInfo.isAnyVicinityBonusYieldChanges():
						for iBonus in xrange(GC.getNumBonusInfos()):
							for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
								aVicinityBonusYieldChanges[REPLACED][iBonus][iYield] += CvReplacedBuildingInfo.getVicinityBonusYieldChanges(iBonus, iYield)
					if CvReplacedBuildingInfo.isAnyBonusYieldModifiers():
						for iBonus in xrange(GC.getNumBonusInfos()):
							for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
								aBonusYieldModifiers[REPLACED][iBonus][iYield] += CvReplacedBuildingInfo.getBonusYieldModifier(iBonus, iYield)
					if CvReplacedBuildingInfo.isAnyBonusCommercePercentChanges():
						for iBonus in xrange(GC.getNumBonusInfos()):
							for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
								aBonusCommercePercentChanges[REPLACED][iBonus][iCommerce] += CvReplacedBuildingInfo.getBonusCommercePercentChanges(iBonus, iCommerce)
					if CvReplacedBuildingInfo.isAnyBonusCommerceModifiers():
						for iBonus in xrange(GC.getNumBonusInfos()):
							for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
								aBonusCommerceModifiers[REPLACED][iBonus][iCommerce] += CvReplacedBuildingInfo.getBonusCommerceModifier(iBonus, iCommerce)

				#Building shouldn't be worse than replaced one!
				for iBonus in xrange(GC.getNumBonusInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if aBonusYieldChanges[BASE][iBonus][iYield] < aBonusYieldChanges[REPLACED][iBonus][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" "+GC.getYieldInfo(iYield).getType()+" Bonus Yield Changes "+str(aBonusYieldChanges[BASE][iBonus])+"/"+str(aBonusYieldChanges[REPLACED][iBonus]))
						if aVicinityBonusYieldChanges[BASE][iBonus][iYield] < aVicinityBonusYieldChanges[REPLACED][iBonus][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" "+GC.getYieldInfo(iYield).getType()+" Bonus vicinity Yield Changes "+str(aVicinityBonusYieldChanges[BASE][iBonus])+"/"+str(aVicinityBonusYieldChanges[REPLACED][iBonus]))
						if aBonusYieldModifiers[BASE][iBonus][iYield] < aBonusYieldModifiers[REPLACED][iBonus][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" "+GC.getYieldInfo(iYield).getType()+" Bonus Yield Modifiers "+str(aBonusYieldModifiers[BASE][iBonus])+"/"+str(aBonusYieldModifiers[REPLACED][iBonus]))
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						if aBonusCommercePercentChanges[BASE][iBonus][iCommerce] < aBonusCommercePercentChanges[REPLACED][iBonus][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" "+GC.getCommerceInfo(iCommerce).getType()+" Bonus Commerce percent Changes "+str(aBonusCommercePercentChanges[BASE][iBonus])+"/"+str(aBonusCommercePercentChanges[REPLACED][iBonus]))
						if aBonusCommerceModifiers[BASE][iBonus][iCommerce] < aBonusCommerceModifiers[REPLACED][iBonus][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" "+GC.getCommerceInfo(iCommerce).getType()+" Bonus Commerce Modifiers "+str(aBonusCommerceModifiers[BASE][iBonus])+"/"+str(aBonusCommerceModifiers[REPLACED][iBonus]))

				#======================================================================================================================================
				#<TechYieldChanges>, <TechYieldModifiers>, <TechCommerceChanges>, <TechCommerceModifiers>, <TechSpecialistChanges> - base
				aTechYieldChanges = [[[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumTechInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aTechYieldModifiers = [[[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumTechInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aTechCommerceChanges = [[[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumTechInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aTechCommerceModifiers = [[[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumTechInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aTechSpecialistChanges = [[[0 for x in xrange(GC.getNumSpecialistInfos())] for y in xrange(GC.getNumTechInfos())] for z in xrange(MAIN_ARRAY_SIZE)]

				for entry in CvBuildingInfo.getTechYieldChanges100():
					aTechYieldChanges[BASE][entry.eTech][entry.eYield] += entry.value
				for entry in CvBuildingInfo.getTechYieldModifiers():
					aTechYieldModifiers[BASE][entry.eTech][entry.eYield] += entry.value
				for entry in CvBuildingInfo.getTechCommerceChanges100():
					aTechCommerceChanges[BASE][entry.eTech][entry.eCommerce] += entry.value
				for entry in CvBuildingInfo.getTechCommerceModifiers():
					aTechCommerceModifiers[BASE][entry.eTech][entry.eCommerce] += entry.value

				if CvBuildingInfo.isAnyTechSpecialistChanges():
					for iTech in xrange(GC.getNumTechInfos()):
						for iSpecialist in xrange(GC.getNumSpecialistInfos()):
							if CvReplacedBuildingInfo.getTechSpecialistChange(iTech, iSpecialist) != 0:
								aTechSpecialistChanges[BASE][iTech][iSpecialist] += CvBuildingInfo.getTechSpecialistChange(iTech, iSpecialist)

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<TechYieldChanges>, <TechYieldModifiers>, <TechCommerceChanges>, <TechCommerceModifiers>, <TechSpecialistChanges>

					for entry in CvReplacedBuildingInfo.getTechYieldChanges100():
						aTechYieldChanges[REPLACED][entry.eTech][entry.eYield] += entry.value
						if GC.getTechInfo(entry.eTech).getGridX() <= iTechBase:
							aYieldChangesList[REPLACED][entry.eYield] += 0.01*entry.value
					for entry in CvReplacedBuildingInfo.getTechYieldModifiers():
						aTechYieldModifiers[REPLACED][entry.eTech][entry.eYield] += entry.value
						if GC.getTechInfo(entry.eTech).getGridX() <= iTechBase:
							aYieldModifiersList[REPLACED][entry.eYield] += entry.value
					for entry in CvReplacedBuildingInfo.getTechCommerceChanges100():
						aTechCommerceChanges[REPLACED][entry.eTech][entry.eCommerce] += entry.value
						if GC.getTechInfo(entry.eTech).getGridX() <= iTechBase:
							aCommerceChanges[REPLACED][entry.eCommerce] += 0.01*entry.value
					for entry in CvReplacedBuildingInfo.getTechCommerceModifiers():
						aTechCommerceModifiers[REPLACED][entry.eTech][entry.eCommerce] += entry.value
						if GC.getTechInfo(entry.eTech).getGridX() <= iTechBase:
							aCommerceModifiers[REPLACED][entry.eCommerce] += entry.value

					if CvReplacedBuildingInfo.isAnyTechSpecialistChanges():
						for iTech in xrange(GC.getNumTechInfos()):
							for iSpecialist in xrange(GC.getNumSpecialistInfos()):
								if CvReplacedBuildingInfo.getTechSpecialistChange(iTech, iSpecialist) != 0:
									aTechSpecialistChanges[REPLACED][iTech][iSpecialist] += CvReplacedBuildingInfo.getTechSpecialistChange(iTech, iSpecialist)
									if GC.getTechInfo(iTech).getGridX() <= iTechBase:
										aFreeSpecialistCounts[REPLACED][iTech][iSpecialist] += CvReplacedBuildingInfo.getTechSpecialistChange(iTech, iSpecialist)

				#Building shouldn't be worse than replaced one!
				for iTech in xrange(GC.getNumTechInfos()):
					if GC.getTechInfo(iTech).getGridX() > iTechBase:
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							if aTechYieldChanges[BASE][iTech][iYield] < aTechYieldChanges[REPLACED][iTech][iYield]:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" "+GC.getYieldInfo(iYield).getType()+" Tech Yield Changes "+str(aTechYieldChanges[BASE][iTech])+"/"+str(aTechYieldChanges[REPLACED][iTech]))
							if aTechYieldModifiers[BASE][iTech][iYield] < aTechYieldModifiers[REPLACED][iTech][iYield]:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" "+GC.getYieldInfo(iYield).getType()+" Tech Yield Modifiers "+str(aTechYieldModifiers[BASE][iTech])+"/"+str(aTechYieldModifiers[REPLACED][iTech]))
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							if aTechCommerceChanges[BASE][iTech][iCommerce] < aTechCommerceChanges[REPLACED][iTech][iCommerce]:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" "+GC.getCommerceInfo(iCommerce).getType()+" Tech Commerce Changes "+str(aTechCommerceChanges[BASE][iTech])+"/"+str(aTechCommerceChanges[REPLACED][iTech]))
							if aTechCommerceModifiers[BASE][iTech][iCommerce] < aTechCommerceModifiers[REPLACED][iTech][iCommerce]:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" "+GC.getCommerceInfo(iCommerce).getType()+" Tech Commerce Modifiers "+str(aTechCommerceModifiers[BASE][iTech])+"/"+str(aTechCommerceModifiers[REPLACED][iTech]))
						for iSpecialist in xrange(GC.getNumSpecialistInfos()):
							if aTechSpecialistChanges[BASE][iTech][iSpecialist] < aTechSpecialistChanges[REPLACED][iTech][iSpecialist]:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech Specialist Changes "+GC.getSpecialistInfo(iSpecialist).getType())

					if GC.getTechInfo(iTech).getGridX() <= iTechBase:
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							if aYieldChangesList[BASE][iYield] < aYieldChangesList[REPLACED][iYield]:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" Yield Changes (Early techs boost) "+str(aYieldChangesList[BASE])+"/"+str(aYieldChangesList[REPLACED]))
							if aYieldModifiersList[BASE][iYield] < aYieldModifiersList[REPLACED][iYield]:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getYieldInfo(iYield).getType()+" Yield Modifiers (Early techs boost) "+str(aYieldModifiersList[BASE])+"/"+str(aYieldModifiersList[REPLACED]))
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							if aCommerceChanges[BASE][iCommerce] < aCommerceChanges[REPLACED][iCommerce]:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getCommerceInfo(iCommerce).getType()+" Commerce Changes (Early techs boost) "+str(aCommerceChanges[BASE])+"/"+str(aCommerceChanges[REPLACED]))
							if aCommerceModifiers[BASE][iCommerce] < aCommerceModifiers[REPLACED][iCommerce]:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getCommerceInfo(iCommerce).getType()+" Commerce Modifiers (Early techs boost) "+str(aCommerceModifiers[BASE])+"/"+str(aCommerceModifiers[REPLACED]))
						for iSpecialist in xrange(GC.getNumSpecialistInfos()):
							if aFreeSpecialistCounts[BASE][iSpecialist] < aFreeSpecialistCounts[REPLACED][iSpecialist] and CvBuildingInfo.getType().find("BUILDING_EMANCIPATION_PROCLAMATION_EFFECT") == -1:
								self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getSpecialistInfo(iSpecialist).getType()+" Free specialists Count (Early techs boost) "+str(aFreeSpecialistCounts[BASE][iSpecialist])+"/"+str(aFreeSpecialistCounts[REPLACED][iSpecialist]))


				#======================================================================================================================================
				#<SpecialistYieldChanges>, <LocalSpecialistCommerceChanges>, <SpecialistCommerceChanges> - base
				aSpecialistYieldChanges = [[[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumSpecialistInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aLocalSpecialistCommerceChanges = [[[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumSpecialistInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				aSpecialistCommerceChanges = [[[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumSpecialistInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				if CvBuildingInfo.isAnySpecialistYieldChanges():
					for iSpecialist in xrange(GC.getNumSpecialistInfos()):
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							aSpecialistYieldChanges[BASE][iSpecialist][iYield] += CvBuildingInfo.getSpecialistYieldChange(iSpecialist, iYield)
				if CvBuildingInfo.isAnySpecialistCommerceChanges():
					for iSpecialist in xrange(GC.getNumSpecialistInfos()):
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							aSpecialistCommerceChanges[BASE][iSpecialist][iCommerce] += CvBuildingInfo.getSpecialistCommerceChange(iSpecialist, iCommerce)
				if CvBuildingInfo.isAnyLocalSpecialistCommerceChanges():
					for iSpecialist in xrange(GC.getNumSpecialistInfos()):
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							aLocalSpecialistCommerceChanges[BASE][iSpecialist][iCommerce] += CvBuildingInfo.getLocalSpecialistCommerceChange(iSpecialist, iCommerce)

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<SpecialistYieldChanges>, <LocalSpecialistCommerceChanges>, <SpecialistCommerceChanges>
					if CvBuildingInfo.isAnySpecialistYieldChanges():
						for iSpecialist in xrange(GC.getNumSpecialistInfos()):
							for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
								aSpecialistYieldChanges[REPLACED][iSpecialist][iYield] += CvReplacedBuildingInfo.getSpecialistYieldChange(iSpecialist, iYield)
					if CvBuildingInfo.isAnySpecialistCommerceChanges():
						for iSpecialist in xrange(GC.getNumSpecialistInfos()):
							for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
								aSpecialistCommerceChanges[REPLACED][iSpecialist][iCommerce] += CvReplacedBuildingInfo.getSpecialistCommerceChange(iSpecialist, iCommerce)
					if CvBuildingInfo.isAnyLocalSpecialistCommerceChanges():
						for iSpecialist in xrange(GC.getNumSpecialistInfos()):
							for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
								aLocalSpecialistCommerceChanges[REPLACED][iSpecialist][iCommerce] += CvReplacedBuildingInfo.getLocalSpecialistCommerceChange(iSpecialist, iCommerce)

				#Building shouldn't be worse than replaced one!
				for iSpecialist in xrange(GC.getNumSpecialistInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if aSpecialistYieldChanges[BASE][iSpecialist][iYield] < aSpecialistYieldChanges[REPLACED][iSpecialist][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getSpecialistInfo(iSpecialist).getType()+" "+GC.getYieldInfo(iYield).getType()+" Specialist Yield Changes "+str(aSpecialistYieldChanges[BASE][iSpecialist])+"/"+str(aSpecialistYieldChanges[REPLACED][iSpecialist]))
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						if aLocalSpecialistCommerceChanges[BASE][iSpecialist][iCommerce] < aLocalSpecialistCommerceChanges[REPLACED][iSpecialist][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getSpecialistInfo(iSpecialist).getType()+" "+GC.getCommerceInfo(iCommerce).getType()+" Local Specialist Commerce Changes "+str(aLocalSpecialistCommerceChanges[BASE][iSpecialist])+"/"+str(aLocalSpecialistCommerceChanges[REPLACED][iSpecialist]))
						if aSpecialistCommerceChanges[BASE][iSpecialist][iCommerce] < aSpecialistCommerceChanges[REPLACED][iSpecialist][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getSpecialistInfo(iSpecialist).getType()+" "+GC.getCommerceInfo(iCommerce).getType()+" Specialist Commerce Changes "+str(aSpecialistCommerceChanges[BASE][iSpecialist])+"/"+str(aSpecialistCommerceChanges[REPLACED][iSpecialist]))

				#==============================================================================================================
				#<TerrainYieldChanges> - base
				aTerrainYieldChanges = [[[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumTerrainInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				for pTerrainYieldChanges in CvBuildingInfo.getTerrainYieldChange():
					iTerrain = pTerrainYieldChanges.eTerrain
					iYield = pTerrainYieldChanges.eYield
					aTerrainYieldChanges[BASE][iTerrain][iYield] += pTerrainYieldChanges.value

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<TerrainYieldChanges>
					for pTerrainYieldChanges in CvReplacedBuildingInfo.getTerrainYieldChange():
						iTerrain = pTerrainYieldChanges.eTerrain
						iYield = pTerrainYieldChanges.eYield
						aTerrainYieldChanges[REPLACED][iTerrain][iYield] += pTerrainYieldChanges.value

				#Building shouldn't be worse than replaced one!
				for iTerrain in xrange(GC.getNumTerrainInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if aTerrainYieldChanges[BASE][iTerrain][iYield] < aTerrainYieldChanges[REPLACED][iTerrain][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTerrainInfo(iTerrain).getType()+" "+GC.getYieldInfo(iYield).getType()+" Terrain Yields "+str(aTerrainYieldChanges[BASE][iTerrain])+"/"+str(aTerrainYieldChanges[REPLACED][iTerrain]))

				#==============================================================================================================
				#<PlotYieldChanges> - base
				aPlotYieldChanges = [[[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(PlotTypes.NUM_PLOT_TYPES)] for z in xrange(MAIN_ARRAY_SIZE)]
				for pPlotYieldChanges in CvBuildingInfo.getPlotYieldChange():
					iPlot = pPlotYieldChanges.iType
					iYield = pPlotYieldChanges.iIndex
					aPlotYieldChanges[BASE][iPlot][iYield] += pPlotYieldChanges.iValue

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<PlotYieldChanges>
					for pPlotYieldChanges in CvReplacedBuildingInfo.getPlotYieldChange():
						iPlot = pPlotYieldChanges.iType
						iYield = pPlotYieldChanges.iIndex
						aPlotYieldChanges[REPLACED][iPlot][iYield] += pPlotYieldChanges.iValue

				#Building shouldn't be worse than replaced one!
				for iPlot in xrange(PlotTypes.NUM_PLOT_TYPES):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if aPlotYieldChanges[BASE][iPlot][iYield] < aPlotYieldChanges[REPLACED][iPlot][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+str(iPlot)+" "+GC.getYieldInfo(iYield).getType()+" Plot Yields "+str(aPlotYieldChanges[BASE][iPlot])+"/"+str(aPlotYieldChanges[REPLACED][iPlot]))
							self.log("We don't have plot type display, so ENUM is displayed. Here is what they represent: PLOT_PEAK, PLOT_HILLS, PLOT_LAND, PLOT_OCEAN")

				#==============================================================================================================
				#<GlobalBuildingExtraCommerces> - base
				aGlobalBuildingExtraCommerces = [[[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBuildingInfos())] for z in xrange(MAIN_ARRAY_SIZE)]
				for pBuildingCommerceChange in CvBuildingInfo.getGlobalBuildingCommerceChanges():
					iAffectedBuilding = pBuildingCommerceChange.eBuilding
					iCommerce = pBuildingCommerceChange.eCommerce
					aGlobalBuildingExtraCommerces[BASE][iAffectedBuilding][iCommerce] += pBuildingCommerceChange.value

				#Analyze replacements by tag
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<GlobalBuildingExtraCommerces>
					for pBuildingCommerceChange in CvReplacedBuildingInfo.getGlobalBuildingCommerceChanges():
						iAffectedBuilding = pBuildingCommerceChange.eBuilding
						iCommerce = pBuildingCommerceChange.eCommerce
						aGlobalBuildingExtraCommerces[REPLACED][iAffectedBuilding][iCommerce] += pBuildingCommerceChange.value

				#Building shouldn't be worse than replaced one!
				for iAffectedBuilding in xrange(GC.getNumBuildingInfos()):
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						if aGlobalBuildingExtraCommerces[BASE][iAffectedBuilding][iCommerce] < aGlobalBuildingExtraCommerces[REPLACED][iAffectedBuilding][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuilding).getType()+" "+GC.getCommerceInfo(iCommerce).getType()+" Global Building extra Commerces "+str(aGlobalBuildingExtraCommerces[BASE][iAffectedBuilding])+"/"+str(aGlobalBuildingExtraCommerces[REPLACED][iAffectedBuilding]))

	#Building - check if building replacements are present in tags affecting buildings
	def checkBuildingReplacingAvailability(self):
		aSpecialBuildingsList = [GC.getInfoTypeForString("BUILDING_POLLUTION_BLACKENEDSKIES"), GC.getInfoTypeForString("BUILDING_GAMBLING_BAN"), GC.getInfoTypeForString("BUILDING_ALCOCHOL_PROHIBITION"), GC.getInfoTypeForString("BUILDING_DRUG_PROHIBITION"), GC.getInfoTypeForString("BUILDING_PROSTITUTION_BAN")]
		#Buildings referencing buildings
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)

			#<GlobalBuildingExtraCommerces>
			for pBuildingCommerceChange in CvBuildingInfo.getGlobalBuildingCommerceChanges():
				iAffectedBuilding = pBuildingCommerceChange.eBuilding
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				aReplacementBuildingsList = []
				if iAffectedBuilding not in aSpecialBuildingsList:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for pBuildingCommerceChange in CvBuildingInfo.getGlobalBuildingCommerceChanges():
					iAffectedBuilding = pBuildingCommerceChange.eBuilding
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if len(aReplacementBuildingsList) > 0:
					self.log(CvBuildingInfo.getType()+" GlobalBuildingExtraCommerces "+CvAffectedBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

			#<GlobalBuildingCostModifiers>
			for iAffectedBuilding, iCostModifier in CvBuildingInfo.getGlobalBuildingCostModifiers():
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				aReplacementBuildingsList = []
				if iAffectedBuilding not in aSpecialBuildingsList:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for iAffectedBuilding, iCostModifier in CvBuildingInfo.getGlobalBuildingCostModifiers():
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if len(aReplacementBuildingsList) > 0:
					self.log(CvBuildingInfo.getType()+" GlobalBuildingCostModifiers "+CvAffectedBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))
				if CvAffectedBuildingInfo.getProductionCost() < 1:
					self.log(CvAffectedBuildingInfo.getType()+" has no cost so shouldn't be listed in GlobalBuildingCostModifiers in "+CvBuildingInfo.getType())

			#<GlobalBuildingProductionModifiers>
			for iAffectedBuilding, iGlobalProductionModifier in CvBuildingInfo.getGlobalBuildingProductionModifiers():
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				aReplacementBuildingsList = []
				if iAffectedBuilding not in aSpecialBuildingsList:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for iAffectedBuilding, iGlobalProductionModifier in CvBuildingInfo.getGlobalBuildingProductionModifiers():
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if len(aReplacementBuildingsList) > 0:
					self.log(CvBuildingInfo.getType()+" GlobalBuildingProductionModifiers "+CvAffectedBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))
				if CvAffectedBuildingInfo.getProductionCost() < 1:
					self.log(CvAffectedBuildingInfo.getType()+" has no cost so shouldn't be listed in GlobalBuildingProductionModifiers in "+CvBuildingInfo.getType())

			#<BuildingHappinessChanges>
			for iAffectedBuilding, iHappiness in CvBuildingInfo.getBuildingHappinessChanges():
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				aReplacementBuildingsList = []
				if iAffectedBuilding not in aSpecialBuildingsList:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for iAffectedBuilding, iHappiness in CvBuildingInfo.getBuildingHappinessChanges():
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if len(aReplacementBuildingsList) > 0:
					self.log(CvBuildingInfo.getType()+" BuildingHappinessChanges "+CvAffectedBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

			#<BuildingProductionModifiers>
			for iAffectedBuilding, iProductionModifier in CvBuildingInfo.getBuildingProductionModifiers():
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				aReplacementBuildingsList = []
				if iAffectedBuilding not in aSpecialBuildingsList:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for iAffectedBuilding, iProductionModifier in CvBuildingInfo.getBuildingProductionModifiers():
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if len(aReplacementBuildingsList) > 0:
					self.log(CvBuildingInfo.getType()+" BuildingProductionModifiers "+CvAffectedBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))
				if CvAffectedBuildingInfo.getProductionCost() < 1:
					self.log(CvAffectedBuildingInfo.getType()+" has no cost so shouldn't be listed in BuildingProductionModifiers in "+CvBuildingInfo.getType())

			#<ImprovementFreeSpecialists> - building references improvements, those potentially can upgrade
			aImprovementUnlistedUpgrades = []
			for iImprovement, iNumFreeSpecialists in CvBuildingInfo.getImprovementFreeSpecialists():
				CvImprovementInfo = GC.getImprovementInfo(iImprovement)
				if CvImprovementInfo.getImprovementUpgrade() != -1:
					aImprovementUnlistedUpgrades.append(GC.getImprovementInfo(CvImprovementInfo.getImprovementUpgrade()).getType())
				for iImprovementReplacement in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					if GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementReplacement)).getType() not in aImprovementUnlistedUpgrades:
						aImprovementUnlistedUpgrades.append(GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementReplacement)).getType())
			#If improvement is listed, then remove it
			for iImprovement, iNumFreeSpecialists in CvBuildingInfo.getImprovementFreeSpecialists():
				if GC.getImprovementInfo(iImprovement).getType() in aImprovementUnlistedUpgrades:
					aImprovementUnlistedUpgrades.remove(GC.getImprovementInfo(iImprovement).getType())
			if len(aImprovementUnlistedUpgrades) > 0:
				self.log(CvBuildingInfo.getType()+" should have improvement upgrades for ImprovementFreeSpecialists "+str(aImprovementUnlistedUpgrades))

			#<PrereqOrImprovement> - Improvement requirement replacement
			aImprovementUnlistedUpgrades = []
			for iImprovement in CvBuildingInfo.getPrereqOrImprovements():
				CvImprovementInfo = GC.getImprovementInfo(iImprovement)
				if CvImprovementInfo.getImprovementUpgrade() != -1:
					aImprovementUnlistedUpgrades.append(GC.getImprovementInfo(CvImprovementInfo.getImprovementUpgrade()).getType())
				for iImprovementReplacement in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					if GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementReplacement)).getType() not in aImprovementUnlistedUpgrades:
						aImprovementUnlistedUpgrades.append(GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementReplacement)).getType())
			#If improvement is listed, then remove it
			for iImprovement in CvBuildingInfo.getPrereqOrImprovements():
				if GC.getImprovementInfo(iImprovement).getType() in aImprovementUnlistedUpgrades:
					aImprovementUnlistedUpgrades.remove(GC.getImprovementInfo(iImprovement).getType())
			if len(aImprovementUnlistedUpgrades) > 0:
				self.log(CvBuildingInfo.getType()+" should have improvement upgrades for PrereqOrImprovement "+str(aImprovementUnlistedUpgrades))

		#Civics referencing buildings
		for iCivic in xrange(GC.getNumCivicInfos()):
			CvCivicInfo = GC.getCivicInfo(iCivic)

			#<BuildingHappinessChanges>
			aBuildingList = []
			if CvCivicInfo.isAnyBuildingHappinessChange():
				for iBuilding in xrange(GC.getNumBuildingInfos()):
					if CvCivicInfo.getBuildingHappinessChanges(iBuilding) != 0:
						aBuildingList.append(iBuilding)

			if len(aBuildingList) > 0:
				#Analyze list of Buildings
				aBuildingUpgrades = []
				for i in xrange(len(aBuildingList)):
					CvBuildingInfo = GC.getBuildingInfo(aBuildingList[i])
					for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
						aBuildingUpgrades.append(CvBuildingInfo.getReplacementBuilding(i))
				#We want nonrepeating list
				aBuildingUniqueUpgrades = []
				for i in xrange(len(aBuildingUpgrades)):
					if aBuildingUpgrades[i] not in aBuildingUniqueUpgrades:
						aBuildingUniqueUpgrades.append(aBuildingUpgrades[i])
				#If Building is listed, then remove it
				for i in xrange(len(aBuildingList)):
					if aBuildingList[i] in aBuildingUniqueUpgrades:
						aBuildingUniqueUpgrades.remove(aBuildingList[i])
				#Get names
				aBuildingsList = []
				for i in xrange(len(aBuildingUniqueUpgrades)):
					aBuildingsList.append(GC.getBuildingInfo(aBuildingUniqueUpgrades[i]).getType())
				if len(aBuildingsList) > 0:
					self.log(CvCivicInfo.getType()+" should have Building upgrades for BuildingHappinessChanges "+str(aBuildingsList))

			#<BuildingHealthChanges>
			aBuildingList = []
			if CvCivicInfo.isAnyBuildingHealthChange():
				for iBuilding in xrange(GC.getNumBuildingInfos()):
					if CvCivicInfo.getBuildingHealthChanges(iBuilding) != 0:
						aBuildingList.append(iBuilding)

			if len(aBuildingList) > 0:
				#Analyze list of Buildings
				aBuildingUpgrades = []
				for i in xrange(len(aBuildingList)):
					CvBuildingInfo = GC.getBuildingInfo(aBuildingList[i])
					for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
						aBuildingUpgrades.append(CvBuildingInfo.getReplacementBuilding(i))
				#We want nonrepeating list
				aBuildingUniqueUpgrades = []
				for i in xrange(len(aBuildingUpgrades)):
					if aBuildingUpgrades[i] not in aBuildingUniqueUpgrades:
						aBuildingUniqueUpgrades.append(aBuildingUpgrades[i])
				#If Building is listed, then remove it
				for i in xrange(len(aBuildingList)):
					if aBuildingList[i] in aBuildingUniqueUpgrades:
						aBuildingUniqueUpgrades.remove(aBuildingList[i])
				#Get names
				aBuildingsList = []
				for i in xrange(len(aBuildingUniqueUpgrades)):
					aBuildingsList.append(GC.getBuildingInfo(aBuildingUniqueUpgrades[i]).getType())
				if len(aBuildingsList) > 0:
					self.log(CvCivicInfo.getType()+" should have Building upgrades for BuildingHealthChanges "+str(aBuildingsList))

			#<BuildingProductionModifiers>
			aBuildingList = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvCivicInfo.getBuildingProductionModifier(iBuilding) != 0:
					aBuildingList.append(iBuilding)
					if GC.getBuildingInfo(iBuilding).getProductionCost() < 1:
						self.log(GC.getBuildingInfo(iBuilding).getType()+" has no cost so shouldn't be listed in BuildingProductionModifiers in "+CvCivicInfo.getType())

			if len(aBuildingList) > 0:
				#Analyze list of Buildings
				aBuildingUpgrades = []
				for i in xrange(len(aBuildingList)):
					CvBuildingInfo = GC.getBuildingInfo(aBuildingList[i])
					for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
						aBuildingUpgrades.append(CvBuildingInfo.getReplacementBuilding(i))
				#We want nonrepeating list
				aBuildingUniqueUpgrades = []
				for i in xrange(len(aBuildingUpgrades)):
					if aBuildingUpgrades[i] not in aBuildingUniqueUpgrades:
						aBuildingUniqueUpgrades.append(aBuildingUpgrades[i])
				#If Building is listed, then remove it
				for i in xrange(len(aBuildingList)):
					if aBuildingList[i] in aBuildingUniqueUpgrades:
						aBuildingUniqueUpgrades.remove(aBuildingList[i])
				#Get names
				aBuildingsList = []
				for i in xrange(len(aBuildingUniqueUpgrades)):
					aBuildingsList.append(GC.getBuildingInfo(aBuildingUniqueUpgrades[i]).getType())
				if len(aBuildingsList) > 0:
					self.log(CvCivicInfo.getType()+" should have Building upgrades for BuildingProductionModifiers "+str(aBuildingsList))

			#<BuildingCommerceModifiers>
			aBuildingList = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
					if CvCivicInfo.getBuildingCommerceModifier(iBuilding, iCommerce) != 0:
						aBuildingList.append(iBuilding)

			if len(aBuildingList) > 0:
				#Analyze list of Buildings
				aBuildingUpgrades = []
				for i in xrange(len(aBuildingList)):
					CvBuildingInfo = GC.getBuildingInfo(aBuildingList[i])
					for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
						aBuildingUpgrades.append(CvBuildingInfo.getReplacementBuilding(i))
				#We want nonrepeating list
				aBuildingUniqueUpgrades = []
				for i in xrange(len(aBuildingUpgrades)):
					if aBuildingUpgrades[i] not in aBuildingUniqueUpgrades:
						aBuildingUniqueUpgrades.append(aBuildingUpgrades[i])
				#If Building is listed, then remove it
				for i in xrange(len(aBuildingList)):
					if aBuildingList[i] in aBuildingUniqueUpgrades:
						aBuildingUniqueUpgrades.remove(aBuildingList[i])
				#Get names
				aBuildingsList = []
				for i in xrange(len(aBuildingUniqueUpgrades)):
					aBuildingsList.append(GC.getBuildingInfo(aBuildingUniqueUpgrades[i]).getType())
				if len(aBuildingsList) > 0:
					self.log(CvCivicInfo.getType()+" should have Building upgrades for BuildingCommerceModifiers "+str(aBuildingsList))

		#Traits referencing buildings
		for iTrait in xrange(GC.getNumTraitInfos()):
			CvTraitInfo = GC.getTraitInfo(iTrait)

			#<BuildingProductionModifierTypes>
			aReplacementBuildingsList = []
			aUniqueReplacementBuildingsList = []
			aBuildingProductionModifierValues = []
			for i in xrange(CvTraitInfo.getNumBuildingProductionModifiers()):
				iAffectedBuilding = CvTraitInfo.getBuildingProductionModifier(i).id
				aBuildingProductionModifierValues.append(CvTraitInfo.getBuildingProductionModifier(i).value)
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				if iAffectedBuilding not in aSpecialBuildingsList:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for i in xrange(CvTraitInfo.getNumBuildingProductionModifiers()):
					iAffectedBuilding = CvTraitInfo.getBuildingProductionModifier(i).id
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if CvAffectedBuildingInfo.getProductionCost() < 1:
					self.log(CvAffectedBuildingInfo.getType()+" has no cost so shouldn't be listed in BuildingProductionModifierTypes in "+CvTraitInfo.getType())
			#Get unique unlisted replacements
			for i in xrange(len(aReplacementBuildingsList)):
				iBuilding = GC.getInfoTypeForString(aReplacementBuildingsList[i])
				if aReplacementBuildingsList[i] not in aUniqueReplacementBuildingsList and GC.getBuildingInfo(iBuilding).getProductionCost() > 0:
					aUniqueReplacementBuildingsList.append(aReplacementBuildingsList[i])
			if len(aUniqueReplacementBuildingsList) > 0:
				self.log(CvTraitInfo.getType()+" BuildingProductionModifierTypes "+str(aUniqueReplacementBuildingsList))
			if len(aBuildingProductionModifierValues) != 0 and min(aBuildingProductionModifierValues) != max(aBuildingProductionModifierValues):
				self.log("Complex Trait "+CvTraitInfo.getType()+" should have same BuildingProductionModifierTypes value for listed buildings")

			#<BuildingHappinessModifierTypes>
			aReplacementBuildingsList = []
			aUniqueReplacementBuildingsList = []
			aBuildingHappinessModifierValues = []
			for iAffectedBuilding, iHappiness in CvTraitInfo.getBuildingHappinessModifiers():
				aBuildingHappinessModifierValues.append(iHappiness)
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				if iAffectedBuilding not in aSpecialBuildingsList:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for iAffectedBuilding, iHappiness in CvTraitInfo.getBuildingHappinessModifiers():
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
			#Get unique unlisted replacements
			for i in xrange(len(aReplacementBuildingsList)):
				if aReplacementBuildingsList[i] not in aUniqueReplacementBuildingsList:
					aUniqueReplacementBuildingsList.append(aReplacementBuildingsList[i])
			if len(aUniqueReplacementBuildingsList) > 0:
				self.log(CvTraitInfo.getType()+" BuildingHappinessModifierTypes "+str(aUniqueReplacementBuildingsList))
			if len(aBuildingHappinessModifierValues) != 0 and min(aBuildingHappinessModifierValues) != max(aBuildingHappinessModifierValues):
				self.log("Complex Trait "+CvTraitInfo.getType()+" should have same BuildingHappinessModifierTypes value for listed buildings")

	#Buildings - free rewards
	def checkBuildingFreeReward(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechID = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[3]
			iObsoleteTechID = self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[1]

			iFreeBuilding = CvBuildingInfo.getFreeBuilding()
			if iFreeBuilding != -1:
				CvFreeBuilding = GC.getBuildingInfo(iFreeBuilding)
				iFreeBuildingTechID = self.HF.checkBuildingTechRequirements(CvFreeBuilding)[3]
				iObsoleteFreeBuildingTechID = self.HF.checkBuildingTechObsoletionLocation(CvFreeBuilding)[1]
				if iObsoleteTechID != iObsoleteFreeBuildingTechID and CvFreeBuilding.getProductionCost() == -1:
					self.log(CvBuildingInfo.getType()+" obsoletes at: "+str(iObsoleteTechID)+", free building "+CvFreeBuilding.getType()+" obsoletes at: "+str(iObsoleteFreeBuildingTechID))
				if iTechID != iFreeBuildingTechID and CvFreeBuilding.getProductionCost() == -1:
					self.log(CvBuildingInfo.getType()+" unlocks at: "+str(iTechID)+", free building "+CvFreeBuilding.getType()+" unlocks at: "+str(iFreeBuildingTechID))

			iFreeAreaBuilding = CvBuildingInfo.getFreeAreaBuilding()
			if iFreeAreaBuilding != -1:
				CvFreeAreaBuilding = GC.getBuildingInfo(iFreeAreaBuilding)
				iFreeBuildingTechID = self.HF.checkBuildingTechRequirements(CvFreeAreaBuilding)[3]
				iObsoleteFreeBuildingTechID = self.HF.checkBuildingTechObsoletionLocation(CvFreeAreaBuilding)[1]
				if iObsoleteTechID != iObsoleteFreeBuildingTechID and CvFreeAreaBuilding.getProductionCost() == -1:
					self.log(CvBuildingInfo.getType()+" obsoletes at: "+str(iObsoleteTechID)+", free area building "+CvFreeAreaBuilding.getType()+" obsoletes at: "+str(iObsoleteFreeBuildingTechID))
				if iTechID != iFreeBuildingTechID and CvFreeAreaBuilding.getProductionCost() == -1:
					self.log(CvBuildingInfo.getType()+" unlocks at: "+str(iTechID)+", free area building "+CvFreeAreaBuilding.getType()+" unlocks at: "+str(iFreeBuildingTechID))

			if CvBuildingInfo.getType().find("BUILDING_FOLKLORE_") != -1 and iBuilding != iFreeBuilding:
				self.log(CvBuildingInfo.getType()+" if its animal/landscape folklore, then it should give itself")

			for i in xrange(CvBuildingInfo.getNumPrereqNotInCityBuildings()):
				iBlockerBuilding = CvBuildingInfo.getPrereqNotInCityBuilding(i)
				CvBlockerBuilding = GC.getBuildingInfo(iBlockerBuilding)
				if iBlockerBuilding == CvBuildingInfo.getFreeBuilding() or iBlockerBuilding == CvBuildingInfo.getFreeAreaBuilding():
					self.log(CvBuildingInfo.getType()+" gives free building, that is blocker "+CvBlockerBuilding.getType())

	#Building bonus requirements
	def checkBuildingBonusRequirements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]
			#<Bonus>BONUS_X
			iBonus = CvBuildingInfo.getPrereqAndBonus()
			if iBonus != -1:
				bonusTechLoc = self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2]
				if bonusTechLoc > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - Singular AND bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<VicinityBonus>BONUS_X
			iBonus = CvBuildingInfo.getPrereqVicinityBonus()
			if iBonus != -1:
				bonusTechLoc = self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2]
				if bonusTechLoc > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - Singular AND vicinity bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<RawVicinityBonus>BONUS_X
			iBonus = CvBuildingInfo.getPrereqRawVicinityBonus()
			if iBonus != -1:
				bonusTechLoc = self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[0]
				if bonusTechLoc > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - Singular AND raw vicinity bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<PrereqBonuses>
			aBonusTechLocList = []
			for iBonus in CvBuildingInfo.getPrereqOrBonuses():
				aBonusTechLocList.append(self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - Earliest OR bonus prereq late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#<PrereqVicinityBonuses>
			aBonusTechLocList = []
			for iBonus in CvBuildingInfo.getPrereqOrVicinityBonuses():
				aBonusTechLocList.append(self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - Earliest OR vicinity bonus prereq late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#<PrereqRawVicinityBonuses>
			aBonusTechLocList = []
			for iBonus in CvBuildingInfo.getPrereqOrRawVicinityBonuses():
				aBonusTechLocList.append(self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[0])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - Earliest OR raw vicinity bonus prereq late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#<ConstructCondition>
			aBonusGOMReqList = []
			for i in range(2):
				aBonusGOMReqList.append([])
			self.HF.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BONUS, aBonusGOMReqList)

			#Analyze GOM AND Bonus reqs
			aBonusTechLocList = []
			for iBonus in xrange(len(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aBonusTechLocList.append(self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBonus]))[2])
			if len(aBonusTechLocList) > 0 and max(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - GOM AND bonus requirements are late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Bonus reqs
			aBonusTechLocList = []
			for iBonus in xrange(len(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aBonusTechLocList.append(self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBonus]))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - GOM OR bonus requirements are late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

	#Building earliest manufacturer on resource tech reveal
	def checkBuildingBonusManufacturerTech(self):
		#Index of array - Bonus ID, array value at index - producers highest tech requirement location
		aBonusList = [-1]*GC.getNumBonusInfos()
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]

			#<ExtraFreeBonuses>
			if CvBuildingInfo.getType().find("_NATURAL_WONDER_") == -1 and iTechLoc != 0: #Ignore producers without tech requirements - those are subdued animal rewards most commonly
				for iBonus, iNumFree in CvBuildingInfo.getFreeBonuses():
					if aBonusList[iBonus] == -1:
						aBonusList[iBonus] = iTechLoc
					elif aBonusList[iBonus] != -1 and aBonusList[iBonus] > iTechLoc:
						aBonusList[iBonus] = iTechLoc

		for iBonus in xrange(len(aBonusList)):
			iBonusTechLoc = self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2]
			if aBonusList[iBonus] != -1 and GC.getBonusInfo(iBonus).getTechCityTrade() != -1 and not GC.getBonusInfo(iBonus).getConstAppearance() > 0:
				if aBonusList[iBonus] - iBonusTechLoc != 0:
					self.log(GC.getBonusInfo(iBonus).getType()+" "+str(iBonusTechLoc)+" Earliest bonus producer located at: "+str(aBonusList[iBonus]))

	#Building requirement extra requirements
	def checkBuildingRequirementTags(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			aBuildingRequirementList = []

			#<PrereqInCityBuildings> - require all buildings in list
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
				aBuildingRequirementList.append(iPrereqBuilding)

			aBuildingRequirementORList = []
			#<PrereqOrBuildings> - require one building in list
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuildingRequirement)
				if iPrereqBuilding not in aBuildingRequirementORList:
					aBuildingRequirementORList.append(iPrereqBuilding)

			#<ConstructCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.HF.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuildingRequirement]
				if iPrereqBuilding not in aBuildingRequirementList:
					aBuildingRequirementList.append(iPrereqBuilding)

			#Analyze GOM OR Building reqs
			aBuildingRequirementGOMORList = []
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuildingRequirement]
				if iPrereqBuilding not in aBuildingRequirementGOMORList:
					aBuildingRequirementGOMORList.append(iPrereqBuilding)

			#Analyze tags
			for iRequirement in xrange(len(aBuildingRequirementList)):
				CvBuildingRequirement = GC.getBuildingInfo(aBuildingRequirementList[iRequirement])
				if CvBuildingInfo.getMinLatitude() < CvBuildingRequirement.getMinLatitude():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built below latitude of "+str(CvBuildingRequirement.getMinLatitude()))
				if CvBuildingInfo.getMaxLatitude() > CvBuildingRequirement.getMaxLatitude():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built above latitude of "+str(CvBuildingRequirement.getMaxLatitude()))
				if CvBuildingInfo.getMinAreaSize() < CvBuildingRequirement.getMinAreaSize():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built below area size "+str(CvBuildingRequirement.getMinAreaSize()))
				if CvBuildingInfo.getPrereqPopulation() < CvBuildingRequirement.getPrereqPopulation():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built below population "+str(CvBuildingRequirement.getPrereqPopulation()))
				if CvBuildingInfo.getNumCitiesPrereq() < CvBuildingRequirement.getNumCitiesPrereq():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built below amount of cities "+str(CvBuildingRequirement.getNumCitiesPrereq()))
				if CvBuildingInfo.getNumTeamsPrereq() < CvBuildingRequirement.getNumTeamsPrereq():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built below amount of teams "+str(CvBuildingRequirement.getNumTeamsPrereq()))
				if CvBuildingInfo.getUnitLevelPrereq() < CvBuildingRequirement.getUnitLevelPrereq():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built below unit level "+str(CvBuildingRequirement.getUnitLevelPrereq()))
				if not CvBuildingInfo.isWater() and CvBuildingRequirement.isWater():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built because of coastal requirement")
				if not CvBuildingInfo.isRiver() and CvBuildingRequirement.isRiver():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built because of river requirement")
				if not CvBuildingInfo.isFreshWater() and CvBuildingRequirement.isFreshWater():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built because of fresh water requirement")
				if not CvBuildingInfo.isPrereqPower() and CvBuildingRequirement.isPrereqPower():
					self.log(CvBuildingInfo.getType()+" requires "+CvBuildingRequirement.getType()+", and can't be built because of electricity requirement")

			#Independently check OR and GOM OR requirements
			aBuildingList = []
			aORMinLatitude = []
			aORMaxLatitude = []
			aORMinAreaSize = []
			aORPrereqPopulation = []
			aORNumCitiesPrereq = []
			aORNumTeamsPrereq = []
			aORUnitLevelPrereq = []
			aORisWater = []
			aORisRiver = []
			aORisFreshWater = []
			aORisPrereqPower = []
			for iRequirement in xrange(len(aBuildingRequirementORList)):
				CvBuildingRequirement = GC.getBuildingInfo(aBuildingRequirementORList[iRequirement])
				aBuildingList.append(CvBuildingRequirement.getType())
				aORMinLatitude.append(CvBuildingRequirement.getMinLatitude())
				aORMaxLatitude.append(CvBuildingRequirement.getMaxLatitude())
				aORMinAreaSize.append(CvBuildingRequirement.getMinAreaSize())
				aORPrereqPopulation.append(CvBuildingRequirement.getPrereqPopulation())
				aORNumCitiesPrereq.append(CvBuildingRequirement.getNumCitiesPrereq())
				aORNumTeamsPrereq.append(CvBuildingRequirement.getNumTeamsPrereq())
				aORUnitLevelPrereq.append(CvBuildingRequirement.getUnitLevelPrereq())
				aORisWater.append(CvBuildingRequirement.isWater())
				aORisRiver.append(CvBuildingRequirement.isRiver())
				aORisFreshWater.append(CvBuildingRequirement.isFreshWater())
				aORisPrereqPower.append(CvBuildingRequirement.isPrereqPower())
			if len(aBuildingList) > 0 and CvBuildingInfo.getMinLatitude() < min(aORMinLatitude):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORMinLatitude.index(min(aORMinLatitude))]+", and can't be built below latitude of "+str(min(aORMinLatitude)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getMaxLatitude() > max(aORMaxLatitude):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORMaxLatitude.index(max(aORMaxLatitude))]+", and can't be built above latitude of "+str(max(aORMaxLatitude)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getMinAreaSize() < min(aORMinAreaSize):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORMinAreaSize.index(min(aORMinAreaSize))]+", and can't be built below area size "+str(min(aORMinAreaSize)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getPrereqPopulation() < min(aORPrereqPopulation):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORPrereqPopulation.index(min(aORPrereqPopulation))]+", and can't be built below population "+str(min(aORPrereqPopulation)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getNumCitiesPrereq() < min(aORNumCitiesPrereq):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORNumCitiesPrereq.index(min(aORNumCitiesPrereq))]+", and can't be built below amount of cities "+str(min(aORNumCitiesPrereq)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getNumTeamsPrereq() < min(aORNumTeamsPrereq):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORNumTeamsPrereq.index(min(aORNumTeamsPrereq))]+", and can't be built below amount of teams "+str(min(aORNumTeamsPrereq)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getUnitLevelPrereq() < min(aORUnitLevelPrereq):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORUnitLevelPrereq.index(min(aORUnitLevelPrereq))]+", and can't be built below unit level "+str(min(aORUnitLevelPrereq)))
			if len(aBuildingList) > 0 and CvBuildingInfo.isWater() < min(aORisWater):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORisWater.index(min(aORisWater))]+", and can't be built because of coastal requirement")
			if len(aBuildingList) > 0 and CvBuildingInfo.isRiver() < min(aORisRiver):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORisRiver.index(min(aORisRiver))]+", and can't be built because of river requirement")
			if len(aBuildingList) > 0 and CvBuildingInfo.isFreshWater() < min(aORisFreshWater):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORisFreshWater.index(min(aORisFreshWater))]+", and can't be built because of fresh water requirement")
			if len(aBuildingList) > 0 and CvBuildingInfo.isPrereqPower() < min(aORisPrereqPower):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORisPrereqPower.index(min(aORisPrereqPower))]+", and can't be built because of electricity requirement")

			aBuildingList = []
			aORMinLatitude = []
			aORMaxLatitude = []
			aORMinAreaSize = []
			aORPrereqPopulation = []
			aORNumCitiesPrereq = []
			aORNumTeamsPrereq = []
			aORUnitLevelPrereq = []
			aORisWater = []
			aORisRiver = []
			aORisFreshWater = []
			aORisPrereqPower = []
			for iRequirement in xrange(len(aBuildingRequirementGOMORList)):
				CvBuildingRequirement = GC.getBuildingInfo(aBuildingRequirementGOMORList[iRequirement])
				aBuildingList.append(CvBuildingRequirement.getType())
				aORMinLatitude.append(CvBuildingRequirement.getMinLatitude())
				aORMaxLatitude.append(CvBuildingRequirement.getMaxLatitude())
				aORMinAreaSize.append(CvBuildingRequirement.getMinAreaSize())
				aORPrereqPopulation.append(CvBuildingRequirement.getPrereqPopulation())
				aORNumCitiesPrereq.append(CvBuildingRequirement.getNumCitiesPrereq())
				aORNumTeamsPrereq.append(CvBuildingRequirement.getNumTeamsPrereq())
				aORUnitLevelPrereq.append(CvBuildingRequirement.getUnitLevelPrereq())
				aORisWater.append(CvBuildingRequirement.isWater())
				aORisRiver.append(CvBuildingRequirement.isRiver())
				aORisFreshWater.append(CvBuildingRequirement.isFreshWater())
				aORisPrereqPower.append(CvBuildingRequirement.isPrereqPower())
			if len(aBuildingList) > 0 and CvBuildingInfo.getMinLatitude() < min(aORMinLatitude):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORMinLatitude.index(min(aORMinLatitude))]+", and can't be built below latitude of "+str(min(aORMinLatitude)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getMaxLatitude() > max(aORMaxLatitude):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORMaxLatitude.index(max(aORMaxLatitude))]+", and can't be built above latitude of "+str(max(aORMaxLatitude)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getMinAreaSize() < min(aORMinAreaSize):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORMinAreaSize.index(min(aORMinAreaSize))]+", and can't be built below area size "+str(min(aORMinAreaSize)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getPrereqPopulation() < min(aORPrereqPopulation):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORPrereqPopulation.index(min(aORPrereqPopulation))]+", and can't be built below population "+str(min(aORPrereqPopulation)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getNumCitiesPrereq() < min(aORNumCitiesPrereq):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORNumCitiesPrereq.index(min(aORNumCitiesPrereq))]+", and can't be built below amount of cities "+str(min(aORNumCitiesPrereq)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getNumTeamsPrereq() < min(aORNumTeamsPrereq):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORNumTeamsPrereq.index(min(aORNumTeamsPrereq))]+", and can't be built below amount of teams "+str(min(aORNumTeamsPrereq)))
			if len(aBuildingList) > 0 and CvBuildingInfo.getUnitLevelPrereq() < min(aORUnitLevelPrereq):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORUnitLevelPrereq.index(min(aORUnitLevelPrereq))]+", and can't be built below unit level "+str(min(aORUnitLevelPrereq)))
			if len(aBuildingList) > 0 and CvBuildingInfo.isWater() < min(aORisWater):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORisWater.index(min(aORisWater))]+", and can't be built because of coastal requirement")
			if len(aBuildingList) > 0 and CvBuildingInfo.isRiver() < min(aORisRiver):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORisRiver.index(min(aORisRiver))]+", and can't be built because of river requirement")
			if len(aBuildingList) > 0 and CvBuildingInfo.isFreshWater() < min(aORisFreshWater):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORisFreshWater.index(min(aORisFreshWater))]+", and can't be built because of fresh water requirement")
			if len(aBuildingList) > 0 and CvBuildingInfo.isPrereqPower() < min(aORisPrereqPower):
				self.log(CvBuildingInfo.getType()+" requires "+aBuildingList[aORisPrereqPower.index(min(aORisPrereqPower))]+", and can't be built because of electricity requirement")

	#Building requirement civic requirements
	def checkBuildingRequirementCivics(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)

			aBuildingCivicAndList = []
			aBuildingCivicOrList = []
			for iCivic in xrange(GC.getNumCivicInfos()):
				if CvBuildingInfo.isPrereqAndCivics(iCivic):
					aBuildingCivicAndList.append(GC.getCivicInfo(iCivic).getType())
				if CvBuildingInfo.isPrereqOrCivics(iCivic):
					aBuildingCivicOrList.append(GC.getCivicInfo(iCivic).getType())

			#<PrereqInCityBuildings> - require all buildings in list
			aBuildingRequirementList = []
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
				aBuildingRequirementList.append(iPrereqBuilding)
			#Find if requirement needs civics
			for i in xrange(len(aBuildingRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingRequirementList[i])
				aCivicAndList = []
				aCivicOrList = []
				for iCivic in xrange(GC.getNumCivicInfos()):
					if CvBuildingRequirementInfo.isPrereqAndCivics(iCivic):
						aCivicAndList.append(GC.getCivicInfo(iCivic).getType())
					if CvBuildingRequirementInfo.isPrereqOrCivics(iCivic):
						aCivicOrList.append(GC.getCivicInfo(iCivic).getType())
				if (len(aCivicAndList) > 0 or len(aCivicOrList) > 0) and (aBuildingCivicAndList != aCivicAndList or aBuildingCivicOrList != aCivicOrList):
					self.log(CvBuildingInfo.getType()+" AND requirement "+CvBuildingRequirementInfo.getType()+" needs AND|OR civics "+str(aCivicAndList)+str(aCivicOrList)+" while checked building needs AND|OR "+str(aBuildingCivicAndList)+str(aBuildingCivicOrList))

			#<PrereqOrBuildings> - require one building in list
			aBuildingRequirementList = []
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuildingRequirement)
				aBuildingRequirementList.append(iPrereqBuilding)
			#Find if requirement needs civics
			for i in xrange(len(aBuildingRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingRequirementList[i])
				aCivicAndList = []
				aCivicOrList = []
				for iCivic in xrange(GC.getNumCivicInfos()):
					if CvBuildingRequirementInfo.isPrereqAndCivics(iCivic):
						aCivicAndList.append(GC.getCivicInfo(iCivic).getType())
					if CvBuildingRequirementInfo.isPrereqOrCivics(iCivic):
						aCivicOrList.append(GC.getCivicInfo(iCivic).getType())
				if (len(aCivicAndList) > 0 or len(aCivicOrList) > 0) and (aBuildingCivicAndList != aCivicAndList or aBuildingCivicOrList != aCivicOrList):
					self.log(CvBuildingInfo.getType()+" OR requirement "+CvBuildingRequirementInfo.getType()+" needs AND|OR civics "+str(aCivicAndList)+str(aCivicOrList)+" while checked building needs AND|OR "+str(aBuildingCivicAndList)+str(aBuildingCivicOrList))

			#<PrereqAmountBuildings> - require all buildings in empire in list
			aBuildingRequirementList = []
			for iPrereqBuilding, iNumRequired in CvBuildingInfo.getPrereqNumOfBuildings():
				aBuildingRequirementList.append(iPrereqBuilding)
			#Find if requirement needs civics
			for i in xrange(len(aBuildingRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingRequirementList[i])
				aCivicAndList = []
				aCivicOrList = []
				for iCivic in xrange(GC.getNumCivicInfos()):
					if CvBuildingRequirementInfo.isPrereqAndCivics(iCivic):
						aCivicAndList.append(GC.getCivicInfo(iCivic).getType())
					if CvBuildingRequirementInfo.isPrereqOrCivics(iCivic):
						aCivicOrList.append(GC.getCivicInfo(iCivic).getType())
				if (len(aCivicAndList) > 0 or len(aCivicOrList) > 0) and (aBuildingCivicAndList != aCivicAndList or aBuildingCivicOrList != aCivicOrList):
					self.log(CvBuildingInfo.getType()+" Empire AND requirement "+CvBuildingRequirementInfo.getType()+" needs AND|OR civics "+str(aCivicAndList)+str(aCivicOrList)+" while checked building needs AND|OR "+str(aBuildingCivicAndList)+str(aBuildingCivicOrList))

			#<ConstructCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.HF.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aBuildingRequirementList = []
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuildingRequirement]
				aBuildingRequirementList.append(iPrereqBuilding)
			#Find if requirement needs civics
			for i in xrange(len(aBuildingRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingRequirementList[i])
				aCivicAndList = []
				aCivicOrList = []
				for iCivic in xrange(GC.getNumCivicInfos()):
					if CvBuildingRequirementInfo.isPrereqAndCivics(iCivic):
						aCivicAndList.append(GC.getCivicInfo(iCivic).getType())
					if CvBuildingRequirementInfo.isPrereqOrCivics(iCivic):
						aCivicOrList.append(GC.getCivicInfo(iCivic).getType())
				if (len(aCivicAndList) > 0 or len(aCivicOrList) > 0) and (aBuildingCivicAndList != aCivicAndList or aBuildingCivicOrList != aCivicOrList):
					self.log(CvBuildingInfo.getType()+" GOM AND requirement "+CvBuildingRequirementInfo.getType()+" needs AND|OR civics "+str(aCivicAndList)+str(aCivicOrList)+" while checked building needs AND|OR "+str(aBuildingCivicAndList)+str(aBuildingCivicOrList))

			#Analyze GOM OR Building reqs
			aBuildingRequirementList = []
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuildingRequirement]
				aBuildingRequirementList.append(iPrereqBuilding)
			#Find if requirement needs civics
			for i in xrange(len(aBuildingRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingRequirementList[i])
				aCivicAndList = []
				aCivicOrList = []
				for iCivic in xrange(GC.getNumCivicInfos()):
					if CvBuildingRequirementInfo.isPrereqAndCivics(iCivic):
						aCivicAndList.append(GC.getCivicInfo(iCivic).getType())
					if CvBuildingRequirementInfo.isPrereqOrCivics(iCivic):
						aCivicOrList.append(GC.getCivicInfo(iCivic).getType())
				if (len(aCivicAndList) > 0 or len(aCivicOrList) > 0) and (aBuildingCivicAndList != aCivicAndList or aBuildingCivicOrList != aCivicOrList):
					self.log(CvBuildingInfo.getType()+" GOM OR requirement "+CvBuildingRequirementInfo.getType()+" needs AND|OR civics "+str(aCivicAndList)+str(aCivicOrList)+" while checked building needs AND|OR "+str(aBuildingCivicAndList)+str(aBuildingCivicOrList))


	#Building - civic requirements and obsoletions
	def checkBuildingCivicRequirements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]

			aCivicAndTechLocList = []
			aCivicOrTechLocList = []
			for iCivic in xrange(GC.getNumCivicInfos()):
				#<PrereqAndCivics> - require all civics in list
				if CvBuildingInfo.isPrereqAndCivics(iCivic):
					iCivicTechLoc = self.HF.checkCivicTechRequirementLocation(GC.getCivicInfo(iCivic))[0]
					aCivicAndTechLocList.append(iCivicTechLoc)

				#<PrereqOrCivics> - require one civics in list
				if CvBuildingInfo.isPrereqOrCivics(iCivic):
					iCivicTechLoc = self.HF.checkCivicTechRequirementLocation(GC.getCivicInfo(iCivic))[0]
					aCivicOrTechLocList.append(iCivicTechLoc)

			if len(aCivicAndTechLocList) > 0 and max(aCivicAndTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" is unlocked before its civic AND requirements "+str(aCivicAndTechLocList)+" "+str(iTechLoc))
			if len(aCivicOrTechLocList) > 0 and min(aCivicOrTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" is unlocked before its earliest OR civic requirement "+str(aCivicOrTechLocList)+" "+str(iTechLoc))

			if len(aCivicAndTechLocList) > 0 or len(aCivicOrTechLocList) > 0:
				if CvBuildingInfo.getObsoleteTech() != -1:
					self.log(CvBuildingInfo.getType()+" civic extension shouldn't obsolete!")
				if CvBuildingInfo.isRequiresActiveCivics() == 0:
					self.log(CvBuildingInfo.getType()+" should be active only when civics are present")
				if CvBuildingInfo.getNumReplacementBuilding() != 0 or CvBuildingInfo.getNumReplacedBuilding() != 0:
					self.log(CvBuildingInfo.getType()+" shouldn't be replaced and shouldn't be replacing stuff, as its civic building")

	#Building - check if building doesn't obsolete before civic is available
	def checkBuildingCivicInfluences(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iBuildingObsoleteTechLoc = self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
			if CvBuildingInfo.getNumReplacedBuilding() == 0 and CvBuildingInfo.getNumReplacementBuilding() == 0: #Redundant civic tags are carried over from replaced buildings
				for iCivic in xrange(GC.getNumCivicInfos()):
					CvCivicInfo = GC.getCivicInfo(iCivic)
					iCivicTechUnlock = self.HF.checkCivicTechRequirementLocation(CvCivicInfo)[0]
					if iBuildingObsoleteTechLoc < iCivicTechUnlock:

						#<BuildingHappinessChanges>
						if CvCivicInfo.getBuildingHappinessChanges(iBuilding) != 0:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvCivicInfo.getType()+" unlock - BuildingHappinessChanges")

						#<BuildingHealthChanges>
						if CvCivicInfo.getBuildingHealthChanges(iBuilding) != 0:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvCivicInfo.getType()+" unlock - BuildingHealthChanges")

						#<BuildingProductionModifiers>
						if CvCivicInfo.getBuildingProductionModifier(iBuilding) != 0:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvCivicInfo.getType()+" unlock - BuildingProductionModifiers")

						#<BuildingCommerceModifiers>
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							if CvCivicInfo.getBuildingCommerceModifier(iBuilding, iCommerce) != 0:
								self.log(CvBuildingInfo.getType()+" obsoletes before "+CvCivicInfo.getType()+" unlock - BuildingCommerceModifiers")

	#Building tech changes and modifiers
	def checkBuildingTechMods(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]
			if CvBuildingInfo.getType().find("BUILDING_FOLKLORE_") == -1: #Folklore is allowed to have techmod on its unlock as we don't have fractional commerce changes
				#Check if Happiness Changes techs don't appear before building can be unlocked or after is obsoleted
				for iTech, iHappiness in CvBuildingInfo.getTechHappinessChanges():
					iTechTLoc = GC.getTechInfo(iTech).getGridX()
					if iTechTLoc <= iTechLoc:
						self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Happiness Changes early tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType())
					elif CvBuildingInfo.getObsoleteTech() != -1 and iTechTLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
						self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Happiness Changes late tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType())

				#Check if Health Changes techs don't appear before building can be unlocked or after is obsoleted
				for iTech, iHealth in CvBuildingInfo.getTechHealthChanges():
					iTechTLoc = GC.getTechInfo(iTech).getGridX()
					if iTechTLoc <= iTechLoc:
						self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Health Changes early tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType())
					elif CvBuildingInfo.getObsoleteTech() != -1 and iTechTLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
						self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Health Changes late tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType())

				#Check if Yield Changes techs don't appear before building can be unlocked or after is obsoleted
				for entry in CvBuildingInfo.getTechYieldChanges100():
					iTech = entry.eTech
					iTechMLoc = GC.getTechInfo(iTech).getGridX()
					if iTechMLoc <= iTechLoc:
						self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Yield Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())
					elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
						self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Yield Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())

				#Check if Yield Modifiers techs don't appear before building can be unlocked or after is obsoleted
				for entry in CvBuildingInfo.getTechYieldModifiers():
					iTech = entry.eTech
					iTechMLoc = GC.getTechInfo(iTech).getGridX()
					if iTechMLoc <= iTechLoc:
						self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Yield Modifiers early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())
					elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
						self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Yield Modifiers late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())

				#Check if Commerce Changes appear before building can be unlocked or after is obsoleted
				for entry in CvBuildingInfo.getTechCommerceChanges100():
					iTech = entry.eTech
					iTechMLoc = GC.getTechInfo(iTech).getGridX()
					if iTechMLoc <= iTechLoc:
						self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Commerce Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())
					elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
						self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Commerce Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())

				#Check if Commerce Modifiers techs don't appear before building can be unlocked or after is obsoleted
				for entry in CvBuildingInfo.getTechCommerceModifiers():
					iTech = entry.eTech
					iTechMLoc = GC.getTechInfo(iTech).getGridX()
					if iTechMLoc <= iTechLoc:
						self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Commerce Modifiers early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())
					elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
						self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Commerce Modifiers late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())

				#Check if Specialist Changes techs don't appear before building can be unlocked or after is obsoleted
				if CvBuildingInfo.isAnyTechSpecialistChanges():
					for iTech in xrange(GC.getNumSpecialistInfos()):
						for iSpecialist in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							if CvBuildingInfo.getTechSpecialistChange(iTech, iSpecialist):
								iTechMLoc = GC.getTechInfo(iTech).getGridX()
								if iTechMLoc <= iTechLoc:
									self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Specialist Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())
								elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
									self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Specialist Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())

	#Building - bonus must be unlocked before building obsoletion so it can affect building at first place
	def checkBuildingBonusTags(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iBuildingObsoleteTechLoc = self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
			if CvBuildingInfo.getNumReplacedBuilding() == 0 and CvBuildingInfo.getNumReplacementBuilding() == 0: #Redundant bonus tags are carried over from replaced buildings
				if CvBuildingInfo.isAnyBonusYieldChanges():
					for iBonus in xrange(GC.getNumBonusInfos()):
						CvBonusInfo = GC.getBonusInfo(iBonus)
						iBonusTechEnable = self.HF.checkBonusTechRequirementLocation(CvBonusInfo)[2]
						if iBuildingObsoleteTechLoc < iBonusTechEnable:
							for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
								#<BonusYieldChanges>
								if CvBuildingInfo.getBonusYieldChanges(iBonus, iYield) != 0:
									self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusYieldChanges")

				if CvBuildingInfo.isAnyBonusYieldModifiers():
					for iBonus in xrange(GC.getNumBonusInfos()):
						CvBonusInfo = GC.getBonusInfo(iBonus)
						iBonusTechEnable = self.HF.checkBonusTechRequirementLocation(CvBonusInfo)[2]
						if iBuildingObsoleteTechLoc < iBonusTechEnable:
							for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
								#<BonusYieldModifiers>
								if CvBuildingInfo.getBonusYieldModifier(iBonus, iYield) != 0:
									self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusYieldModifiers")

				if CvBuildingInfo.isAnyVicinityBonusYieldChanges():
					for iBonus in xrange(GC.getNumBonusInfos()):
						CvBonusInfo = GC.getBonusInfo(iBonus)
						iBonusTechEnable = self.HF.checkBonusTechRequirementLocation(CvBonusInfo)[2]
						if iBuildingObsoleteTechLoc < iBonusTechEnable:
							for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
								#<VicinityBonusYieldChanges>
								if CvBuildingInfo.getVicinityBonusYieldChanges(iBonus, iYield) != 0:
									self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - VicinityBonusYieldChanges")

				if CvBuildingInfo.isAnyBonusCommerceModifiers():
					for iBonus in xrange(GC.getNumBonusInfos()):
						CvBonusInfo = GC.getBonusInfo(iBonus)
						iBonusTechEnable = self.HF.checkBonusTechRequirementLocation(CvBonusInfo)[2]
						if iBuildingObsoleteTechLoc < iBonusTechEnable:
							for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
								#<BonusCommerceModifiers>
								if CvBuildingInfo.getBonusCommerceModifier(iBonus, iCommerce) != 0:
									self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusCommerceModifiers")

				if CvBuildingInfo.isAnyBonusCommercePercentChanges():
					for iBonus in xrange(GC.getNumBonusInfos()):
						CvBonusInfo = GC.getBonusInfo(iBonus)
						iBonusTechEnable = self.HF.checkBonusTechRequirementLocation(CvBonusInfo)[2]
						if iBuildingObsoleteTechLoc < iBonusTechEnable:
							for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
								#<BonusCommercePercentChanges>
								if CvBuildingInfo.getBonusCommercePercentChanges(iBonus, iCommerce) != 0:
									self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - getBonusCommercePercentChanges")

				for iBonus in xrange(GC.getNumBonusInfos()):
					CvBonusInfo = GC.getBonusInfo(iBonus)
					iBonusTechEnable = self.HF.checkBonusTechRequirementLocation(CvBonusInfo)[2]
					if iBuildingObsoleteTechLoc < iBonusTechEnable:
						#<BonusHappinessChanges>
						if iBonus in CvBuildingInfo.getBonusHappinessChanges():
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusHappinessChanges")

						#<BonusHealthChanges>
						if iBonus in CvBuildingInfo.getBonusHealthChanges():
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusHealthChanges")

						#<BonusProductionModifiers>
						if CvBuildingInfo.getBonusProductionModifier(iBonus) != 0:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusProductionModifiers")

	#Buildings X -> Y: X shouldn't be obsolete before Y is available, and X should be unlocked before Y is obsolete
	def checkBuildingAffectingBuildings(self):
		aAffectedBuildingTechUnlockList = []
		aAffectedBuildingTechObsoletionList = []
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			if CvBuildingInfo.getNumReplacedBuilding() == 0 and CvBuildingInfo.getNumReplacementBuilding() == 0: #Redundant building tags are carried over from replaced buildings
				aAffectedBuildingTechUnlockList.append(self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0])
				aAffectedBuildingTechObsoletionList.append(self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0])
			else:
				aAffectedBuildingTechUnlockList.append(0)
				aAffectedBuildingTechObsoletionList.append(999)

		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvAffectingBuildingInfo = GC.getBuildingInfo(iBuilding)
			iAffectingBuildingUnlockTechLoc = self.HF.checkBuildingTechRequirements(CvAffectingBuildingInfo)[0]
			iAffectingBuildingObsoleteTechLoc = self.HF.checkBuildingTechObsoletionLocation(CvAffectingBuildingInfo)[0]

			#<GlobalBuildingExtraCommerces>
			for pBuildingCommerceChange in CvAffectingBuildingInfo.getGlobalBuildingCommerceChanges():
				i = pBuildingCommerceChange.eBuilding
				if iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[i] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[i]:
					CvAffectedBuildingInfo = GC.getBuildingInfo(i)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - GlobalBuildingExtraCommerces")

			#<GlobalBuildingCostModifiers>
			for iAffectedBuilding, iCostModifier in CvAffectingBuildingInfo.getGlobalBuildingCostModifiers():
				if iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[iAffectedBuilding] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[iAffectedBuilding]:
					CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - GlobalBuildingProductionModifiers")

			#<GlobalBuildingProductionModifiers>
			for iAffectedBuilding, iGlobalProductionModifier in CvAffectingBuildingInfo.getGlobalBuildingProductionModifiers():
				if iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[iAffectedBuilding] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[iAffectedBuilding]:
					CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - GlobalBuildingProductionModifiers")

			#<BuildingHappinessChanges>
			for iAffectedBuilding, iHappiness in CvAffectingBuildingInfo.getBuildingHappinessChanges():
				if iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[iAffectedBuilding] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[iAffectedBuilding]:
					CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - BuildingHappinessChanges")

			#<BuildingProductionModifiers>
			for iAffectedBuilding, iProductionModifier in CvAffectingBuildingInfo.getBuildingProductionModifiers():
				if iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[iAffectedBuilding] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[iAffectedBuilding]:
					CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - BuildingProductionModifiers")

			#<PrereqNotInCityBuildings>
			for i in xrange(CvAffectingBuildingInfo.getNumPrereqNotInCityBuildings()):
				iAffectedBuilding = CvAffectingBuildingInfo.getPrereqNotInCityBuilding(i)
				if iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[iAffectedBuilding] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[iAffectedBuilding]:
					CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - PrereqNotInCityBuildings")

			#<ExtendsBuilding>
			iAffectedBuilding = CvAffectingBuildingInfo.getExtendsBuilding()
			if iAffectedBuilding != -1:
				if iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[iAffectedBuilding] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[iAffectedBuilding]:
					CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - ExtendsBuilding")

	#Building - ensure that building religious tags always reference same religion
	def checkBuildingReligionRequirement(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iRelPrereq1 = CvBuildingInfo.getPrereqReligion()
			iRelPrereq2 = CvBuildingInfo.getReligionType()
			iRelPrereq3 = CvBuildingInfo.getPrereqStateReligion()

			if iRelPrereq1 != -1 and iRelPrereq2 != -1 and iRelPrereq1 != iRelPrereq2:
				self.log(CvBuildingInfo.getType()+" mismatch between getPrereqReligion and getReligionType: "+GC.getReligionType(iRelPrereq1).getType()+" "+GC.getReligionType(iRelPrereq2).getType())
			if iRelPrereq2 != -1 and iRelPrereq3 != -1 and iRelPrereq2 != iRelPrereq3:
				self.log(CvBuildingInfo.getType()+" mismatch between getReligionType and getPrereqStateReligion: "+GC.getReligionType(iRelPrereq2).getType()+" "+GC.getReligionType(iRelPrereq3).getType())
			if iRelPrereq3 != -1 and iRelPrereq1 != -1 and iRelPrereq3 != iRelPrereq1:
				self.log(CvBuildingInfo.getType()+" mismatch between getPrereqStateReligion and getPrereqReligion: "+GC.getReligionType(iRelPrereq3).getType()+" "+GC.getReligionType(iRelPrereq1).getType())

	#Building - variety of tags
	def checkBuildingTags(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			#Commerce double time - wonders only, and ensure flat change exists
			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.getCommerceChangeDoubleTime(iCommerce) != 0 and not (isWorldWonder(iBuilding) or isNationalWonder(iBuilding) or CvBuildingInfo.getHolyCity() != -1):
					self.log(CvBuildingInfo.getType()+" Non-wonder has commerce change double time")

			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.getCommerceChangeDoubleTime(iCommerce) != 0 and CvBuildingInfo.getCommerceChange(iCommerce) == 0:
					self.log(CvBuildingInfo.getType()+" has commerce change double time but no relevant flat commerce change")

			#Hurry modifiers works only on buildable buildings
			if CvBuildingInfo.getProductionCost() == -1 and (CvBuildingInfo.getHurryCostModifier() != 0 or CvBuildingInfo.getHurryAngerModifier() != 0):
				self.log(CvBuildingInfo.getType()+" can't be hurried at first place")

			#GreatPeopleRateChange must be present, if GreatPeopleUnitType is present
			if CvBuildingInfo.getGreatPeopleUnitType() != -1 and CvBuildingInfo.getGreatPeopleRateChange() == 0:
				self.log(CvBuildingInfo.getType()+" has zero rate change of "+GC.getUnitInfo(CvBuildingInfo.getGreatPeopleUnitType()).getType())

			#FreeBonus must be present if iNumFreeBonuses is present and viceversa.
			for iBonus, iNumFree in CvBuildingInfo.getFreeBonuses():
				if iBonus != -1 and iNumFree == 0:
					self.log(CvBuildingInfo.getType()+" has extra free bonus tag, but actually doesn't give bonus")
				if iBonus == -1 and iNumFree != 0:
					self.log(CvBuildingInfo.getType()+" has unspecified extra free bonus type")

			#GlobalBuildingExtraCommerces is meant to be placed on wonders
			for pBuildingCommerceChange in CvBuildingInfo.getGlobalBuildingCommerceChanges():
				CvTargetBuilding = GC.getBuildingInfo(pBuildingCommerceChange.eBuilding)
				if not (isWorldWonder(iBuilding) or isNationalWonder(iBuilding) or CvBuildingInfo.getHolyCity() != -1):
					self.log(CvBuildingInfo.getType()+" GlobalBuildingExtraCommerces should be swapped with "+CvTargetBuilding.getType())

			#ObsoletesToBuilding shouldn't be used, if building doesn't obsolete at first place
			if CvBuildingInfo.getObsoletesToBuilding() != -1 and CvBuildingInfo.getObsoleteTech() == -1:
				self.log(CvBuildingInfo.getType()+" has obsoletion to building defined, but not obsoleteing tech")

	#Check if buildings don't take too much gold for its output
	def checkBuildingMaxMaint(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)

			if CvBuildingInfo.getCommerceChange(0) < 0 and CvBuildingInfo.getProductionCost() > 0: #This building takes gold and is buildable
				iBuildingValue = -CvBuildingInfo.getCommerceChange(0) #Gold change will be added to this

				for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
					iBuildingValue += CvBuildingInfo.getYieldChange(iYield)
					iBuildingValue += CvBuildingInfo.getRiverPlotYieldChange(iYield)
					iBuildingValue += 2*CvBuildingInfo.getYieldModifier(iYield)
					iBuildingValue += 2*CvBuildingInfo.getPowerYieldModifier(iYield)
				for entry in CvBuildingInfo.getTechYieldChanges100():
					iBuildingValue += 0.01*entry.value
				for entry in CvBuildingInfo.getTechYieldModifiers():
					iBuildingValue += 2*entry.value
				if CvBuildingInfo.isAnyBonusYieldChanges():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							iBuildingValue += CvBuildingInfo.getBonusYieldChanges(iBonus, iYield)
				if CvBuildingInfo.isAnyVicinityBonusYieldChanges():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							iBuildingValue += CvBuildingInfo.getVicinityBonusYieldChanges(iBonus, iYield)
				if CvBuildingInfo.isAnyBonusYieldModifiers():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							iBuildingValue += 2*CvBuildingInfo.getBonusYieldModifier(iBonus, iYield)
				for pPlotYieldChanges in CvBuildingInfo.getPlotYieldChange():
					iBuildingValue += pPlotYieldChanges.iValue

				for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
					iBuildingValue += CvBuildingInfo.getCommerceChange(iCommerce)
					iBuildingValue += 2*CvBuildingInfo.getCommerceModifier(iCommerce)
				for entry in CvBuildingInfo.getTechCommerceChanges100():
					iBuildingValue += 0.01*entry.value
				for entry in CvBuildingInfo.getTechCommerceModifiers():
					iBuildingValue += 2*entry.value
				if CvBuildingInfo.isAnyBonusCommercePercentChanges():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							iBuildingValue += CvBuildingInfo.getBonusCommercePercentChanges(iBonus, iCommerce)
				if CvBuildingInfo.isAnyBonusCommerceModifiers():
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							iBuildingValue += 2*CvBuildingInfo.getBonusCommerceModifier(iBonus, iCommerce)

				iBuildingValue += CvBuildingInfo.getGreatPeopleRateChange()
				iBuildingValue += 2*CvBuildingInfo.getGreatPeopleRateModifier()

				for iSpecialist in xrange(GC.getNumSpecialistInfos()):
					iBuildingValue += 5*CvBuildingInfo.getSpecialistCount(iSpecialist)
					iBuildingValue += 5*CvBuildingInfo.getFreeSpecialistCount(iSpecialist)

					if CvBuildingInfo.isAnyTechSpecialistChanges():
						for iTech in xrange(GC.getNumTechInfos()):
							if CvBuildingInfo.getTechSpecialistChange(iTech, iSpecialist) > 0:
								iBuildingValue += 5*CvBuildingInfo.getTechSpecialistChange(iTech, iSpecialist)
					else:
						break

				iBuildingValue += CvBuildingInfo.getHappiness()
				iBuildingValue += CvBuildingInfo.getHealth()

				for iBonus, iHappiness in CvBuildingInfo.getBonusHappinessChanges():
					iBuildingValue += iHappiness
				for iBonus, iHealth in CvBuildingInfo.getBonusHealthChanges():
					iBuildingValue += iHealth
				for iBonus, iNumFree in CvBuildingInfo.getFreeBonuses():
					iBuildingValue += 1

				for iTech, iHappiness in CvBuildingInfo.getTechHappinessChanges():
					iBuildingValue += iHappiness
				for iTech, iHealth in CvBuildingInfo.getTechHealthChanges():
					iBuildingValue += iHealth

				if 3*iBuildingValue < -CvBuildingInfo.getCommerceChange(0):
					self.log(CvBuildingInfo.getType()+" gold maint: "+str(CvBuildingInfo.getCommerceChange(0))+" Suggested max: "+str(-3*iBuildingValue))

	#Check if buildings with yield income have at least minimum yield as derived from era
	def checkBuildingMinYields(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iEra = self.HF.checkBuildingEra(CvBuildingInfo)

			for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
				iYieldChange = CvBuildingInfo.getYieldChange(iYield)

				if iYieldChange > 0 and GC.getInfoTypeForString("MAPCATEGORY_EARTH") in CvBuildingInfo.getMapCategories():
					if iYieldChange < 1 and (iEra == 0 or iEra == 1): # Prehistoric/Ancient
						self.log(CvBuildingInfo.getType()+" - "+GC.getEraInfo(iEra).getType()+" should have at least 1 "+GC.getYieldInfo(iYield).getType())
					if iYieldChange < 2 and (iEra == 2 or iEra == 3): # Classical/Medieval
						self.log(CvBuildingInfo.getType()+" - "+GC.getEraInfo(iEra).getType()+" should have at least 2 "+GC.getYieldInfo(iYield).getType())
					if iYieldChange < 3 and (iEra == 4 or iEra == 5): # Renaissance/Industrial
						self.log(CvBuildingInfo.getType()+" - "+GC.getEraInfo(iEra).getType()+" should have at least 3 "+GC.getYieldInfo(iYield).getType())
					if iYieldChange < 4 and (iEra == 6 or iEra == 7): # Atomic/Information
						self.log(CvBuildingInfo.getType()+" - "+GC.getEraInfo(iEra).getType()+" should have at least 4 "+GC.getYieldInfo(iYield).getType())
					if iYieldChange < 5 and (iEra == 8 or iEra == 9): # Nanotech/Transhuman
						self.log(CvBuildingInfo.getType()+" - "+GC.getEraInfo(iEra).getType()+" should have at least 5 "+GC.getYieldInfo(iYield).getType())
					if iYieldChange < 6 and (iEra == 10 or iEra == 11): # Galactic/Cosmic
						self.log(CvBuildingInfo.getType()+" - "+GC.getEraInfo(iEra).getType()+" should have at least 6 "+GC.getYieldInfo(iYield).getType())
					if iYieldChange < 7 and iEra == 12: # Transcendent
						self.log(CvBuildingInfo.getType()+" - "+GC.getEraInfo(iEra).getType()+" should have at least 7 "+GC.getYieldInfo(iYield).getType())

	#Check if buildings with tech yield/commerce changes have minimum gains from techs: Ancient/Classical/... - 2/3/...
	def checkBuildingTechChanges(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iEra = self.HF.checkBuildingEra(CvBuildingInfo)

			if CvBuildingInfo.getType().find("BUILDING_FOLKLORE") == -1:
				for entry in CvBuildingInfo.getTechYieldChanges100():
					iTechEra = GC.getTechInfo(entry.eTech).getEra()+1 #Prehistoric enum is 0, so shifting it by 1.
					if entry.value > 0 and entry.value < 100*iTechEra:
						self.log(CvBuildingInfo.getType()+" tech yield changes "+GC.getTechInfo(entry.eTech).getType()+" "+GC.getYieldInfo(entry.eYield).getType()+" current/excepted "+str(entry.value)+"/"+str(100*iTechEra))

				for entry in CvBuildingInfo.getTechCommerceChanges100():
					iTechEra = GC.getTechInfo(entry.eTech).getEra()+1
					if entry.value > 0 and entry.value < 100*iTechEra:
						self.log(CvBuildingInfo.getType()+" tech commerce changes "+GC.getTechInfo(entry.eTech).getType()+" "+GC.getCommerceInfo(entry.eCommerce).getType()+" current/excepted "+str(entry.value)+"/"+str(100*iTechEra))

	#Tech - Check if techs have proper costs
	def checkTechCosts(self):
		aBaseCostList = [0, 3, 4, 5, 7, 9, 11, 13, 15, 17, 20, 23, 26, 30, 34, 39, 45, 52, 60, 74, 84, 96, 110, 126, 144, 164, 187, 213, 242, 274, 309, 348, 401, 448, 499, 554, 614, 679, 749, 824, 904, 1040, 1132, 1230, 1334, 1445, 1563, 1688, 1820, 1960, 2208, 2364, 2528, 2701, 2883, 3074, 3274, 3484, 3704, 3934, 4174, 4925, 5187, 5460, 5745, 6042, 6351, 6673, 7008, 7356, 7718, 9094, 9484, 9889, 10309, 10744, 11196, 11665, 12152, 12657, 13181, 13724, 14287, 14870, 15474, 16099, 19246, 19916, 20610, 21329, 22074, 22847, 23650, 24485, 25354, 26259, 27203, 28189, 29220, 35299, 36429, 37614, 38858, 40165, 41539, 42984, 44505, 46107, 47795, 49574, 61449, 63426, 65511, 67710, 70029, 72474, 75052, 77770, 80635, 83654, 86834, 90183, 93709, 122420, 126324, 130429, 134744, 139278, 144040, 149039, 154284, 159944, 166029, 172549, 179514, 186934, 194821, 253187, 262044, 271404, 281279, 291683, 302630, 314134, 326209, 338869, 352130, 366008, 480519, 495679, 511504, 528012, 545221, 563149, 581814, 601234, 621429, 642419, 664224, 686864, 960359] #Tech cost depends on its GridX
		for iTech in xrange(GC.getNumTechInfos()):
			CvTechInfo = GC.getTechInfo(iTech)

			if CvTechInfo.getResearchCost() != aBaseCostList[CvTechInfo.getGridX()]:
				self.log(CvTechInfo.getType()+" should have Cost of "+str(aBaseCostList[CvTechInfo.getGridX()]))

	#Building - Check if buildings have proper costs
	def checkBuildingCosts(self):
		aBaseCostList = [5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 56, 62, 68, 74, 80, 86, 92, 98, 104, 111, 118, 125, 132, 139, 146, 153, 160, 168, 176, 184, 192, 200, 208, 216, 225, 234, 243, 252, 261, 270, 280, 290, 300, 310, 320, 332, 344, 356, 368, 380, 395, 410, 425, 440, 455, 475, 495, 515, 535, 555, 580, 605, 630, 655, 680, 710, 740, 770, 800, 830, 870, 910, 950, 990, 1030, 1080, 1130, 1180, 1230, 1280, 1345, 1410, 1475, 1540, 1605, 1685, 1765, 1845, 1925, 2005, 2105, 2205, 2305, 2405, 2505, 2625, 2745, 2865, 2985, 3105, 3245, 3385, 3525, 3665, 3805, 3975, 4145, 4315, 4485, 4655, 4855, 5055, 5255, 5455, 5655, 5905, 6155, 6405, 6655, 6905, 7205, 7505, 7805, 8105, 8405, 8755, 9105, 9455, 9805, 10155, 10555, 10955, 11355, 11755, 12155, 12655, 13155, 13655, 14155, 14655, 15255, 15855, 16455, 17055, 17655, 18355, 19055, 19755, 20455, 21155, 21955, 22755, 23555, 24355, 25155, 26055, 26955, 27855, 28755, 29655, 30655] #Building cost depend on most advanced tech to unlock. *2 for National, *4 for Group, *6 for World wonders, *8 for Projects
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)

			if CvBuildingInfo.getProductionCost() > 0 and GC.getInfoTypeForString("MAPCATEGORY_EARTH") in CvBuildingInfo.getMapCategories() and iBuilding != GC.getInfoTypeForString("BUILDING_PALACE") and CvBuildingInfo.getType().find("_CULTURE_LOCAL_") == -1:
				iTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0] #Pick one of most advanced: Main/TechTypes/Religious/Special/GOM OR (earliest)
				iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()

				#Regular
				if not isNationalWonder(iBuilding) and not isWorldWonder(iBuilding) and CvBuildingInfo.getProductionCost() != aBaseCostList[iTechLoc]:
					self.log(CvBuildingInfo.getType()+" should have Cost of "+str(aBaseCostList[iTechLoc]))

				#National Wonder
				if isNationalWonder(iBuilding) and CvBuildingInfo.getProductionCost() != 2*aBaseCostList[iTechLoc]:
					self.log(CvBuildingInfo.getType()+" should have Cost of "+str(2*aBaseCostList[iTechLoc]))

				#Group Wonder
				if iSpecialBuilding != -1 and GC.getSpecialBuildingInfo(iSpecialBuilding).getType().find("_GROUP_") != -1 and CvBuildingInfo.getProductionCost() != 4*aBaseCostList[iTechLoc]:
					self.log(CvBuildingInfo.getType()+" should have Cost of "+str(4*aBaseCostList[iTechLoc]))

				#World Wonder
				if isWorldWonder(iBuilding) and not (iSpecialBuilding != -1 and GC.getSpecialBuildingInfo(iSpecialBuilding).getType().find("_GROUP_") != -1) and CvBuildingInfo.getProductionCost() != 6*aBaseCostList[iTechLoc]:
					self.log(CvBuildingInfo.getType()+" should have Cost of "+str(6*aBaseCostList[iTechLoc]))

		#Project
		for iProject in xrange(GC.getNumProjectInfos()):
			CvProjectInfo = GC.getProjectInfo(iProject)
			iTechLoc = GC.getTechInfo(CvProjectInfo.getTechPrereq()).getGridX()

			if CvProjectInfo.getProductionCost() != 8*aBaseCostList[iTechLoc]:
				self.log(CvProjectInfo.getType()+" should have Cost of "+str(8*aBaseCostList[iTechLoc]))

	#Unit - Check if Units have sane costs
	def checkUnitCosts(self):
		aBaseCostList = [5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 56, 62, 68, 74, 80, 86, 92, 98, 104, 111, 118, 125, 132, 139, 146, 153, 160, 168, 176, 184, 192, 200, 208, 216, 225, 234, 243, 252, 261, 270, 280, 290, 300, 310, 320, 332, 344, 356, 368, 380, 395, 410, 425, 440, 455, 475, 495, 515, 535, 555, 580, 605, 630, 655, 680, 710, 740, 770, 800, 830, 870, 910, 950, 990, 1030, 1080, 1130, 1180, 1230, 1280, 1345, 1410, 1475, 1540, 1605, 1685, 1765, 1845, 1925, 2005, 2105, 2205, 2305, 2405, 2505, 2625, 2745, 2865, 2985, 3105, 3245, 3385, 3525, 3665, 3805, 3975, 4145, 4315, 4485, 4655, 4855, 5055, 5255, 5455, 5655, 5905, 6155, 6405, 6655, 6905, 7205, 7505, 7805, 8105, 8405, 8755, 9105, 9455, 9805, 10155, 10555, 10955, 11355, 11755, 12155, 12655, 13155, 13655, 14155, 14655, 15255, 15855, 16455, 17055, 17655, 18355, 19055, 19755, 20455, 21155, 21955, 22755, 23555, 24355, 25155, 26055, 26955, 27855, 28755, 29655, 30655] #Unit cost depend on most advanced tech to unlock.
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)

			bAndSpaceRequirement = False
			for iBuilding in xrange(CvUnitInfo.getNumPrereqAndBuildings()):
				if GC.getInfoTypeForString("MAPCATEGORY_EARTH") not in GC.getBuildingInfo(CvUnitInfo.getPrereqAndBuilding(iBuilding)).getMapCategories():
					bAndSpaceRequirement = True
					break
			bOrSpaceRequirement = False
			if CvUnitInfo.getPrereqOrBuildingsNum() > 0:
				bOrSpaceRequirement = True
				for iBuilding in xrange(CvUnitInfo.getPrereqOrBuildingsNum()):
					if GC.getInfoTypeForString("MAPCATEGORY_EARTH") in GC.getBuildingInfo(CvUnitInfo.getPrereqOrBuilding(iBuilding)).getMapCategories():
						bOrSpaceRequirement = False
						break

			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.HF.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			bGOMAndSpaceRequirement = False
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				if GC.getInfoTypeForString("MAPCATEGORY_EARTH") not in GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]).getMapCategories():
					bGOMAndSpaceRequirement = True
					break
			bGOMOrSpaceRequirement = False
			if len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR]) > 0:
				bGOMOrSpaceRequirement = True
				for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
					if GC.getInfoTypeForString("MAPCATEGORY_EARTH") in GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]).getMapCategories():
						bGOMOrSpaceRequirement = False
						break

			if CvUnitInfo.getProductionCost() > 0 and GC.getInfoTypeForString("MAPCATEGORY_EARTH") in CvUnitInfo.getMapCategories() and bAndSpaceRequirement + bOrSpaceRequirement + bGOMAndSpaceRequirement + bGOMOrSpaceRequirement == 0:
				iTechLoc = self.HF.checkUnitTechRequirementLocation(CvUnitInfo)[0]

				if CvUnitInfo.getType().find("_EXECUTIVE") == -1 and CvUnitInfo.getType().find("_MISSIONARY") == -1 and CvUnitInfo.getType().find("UNIT_UNDEAD_WORKER") == -1:
					if CvUnitInfo.getProductionCost() < 0.75*aBaseCostList[iTechLoc] and CvUnitInfo.getMaxGlobalInstances() == -1:
						self.log(CvUnitInfo.getType()+" is way too cheap, actual cost/min cost derived from XGrid: "+str(CvUnitInfo.getProductionCost())+"/"+str(1+(0.75*aBaseCostList[iTechLoc])))

					if CvUnitInfo.getProductionCost() > 1.25*aBaseCostList[iTechLoc] and CvUnitInfo.getMaxGlobalInstances() == -1:
						self.log(CvUnitInfo.getType()+" is way too expensive, actual cost/max cost derived from XGrid: "+str(CvUnitInfo.getProductionCost())+"/"+str(1.25*aBaseCostList[iTechLoc]))

					if CvUnitInfo.getProductionCost() != 4*aBaseCostList[iTechLoc] and CvUnitInfo.getMaxGlobalInstances() != -1:
						self.log(CvUnitInfo.getType()+" is global unit and should have 4x cost derived from XGrid: "+str(CvUnitInfo.getProductionCost())+"/"+str(4*aBaseCostList[iTechLoc]))

	#Unit - check unit upgrades
	def checkUnitUpgrades(self):
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			iTechLoc = self.HF.checkUnitTechRequirementLocation(CvUnitInfo)[0]
			iTechObs = self.HF.checkUnitTechObsoletionLocation(CvUnitInfo)[0]

			if CvUnitInfo.getNumUnitUpgrades() > 0:
				for iUnitUpgrade in xrange(CvUnitInfo.getNumUnitUpgrades()):
					CvUnitUpgradeInfo = GC.getUnitInfo(CvUnitInfo.getUnitUpgrade(iUnitUpgrade))
					iTechUpgradeLoc = self.HF.checkUnitTechRequirementLocation(CvUnitUpgradeInfo)[0]
					iTechUpgradeObs = self.HF.checkUnitTechObsoletionLocation(CvUnitUpgradeInfo)[0]

					#Ignore neanderthal and post apocalyptic units
					if iTechLoc >= iTechUpgradeLoc and CvUnitInfo.getType().find("UNIT_NEANDERTHAL_",0,17) == -1 and CvUnitUpgradeInfo.getType().find("UNIT_NEANDERTHAL",0,16) == -1 and CvUnitInfo.getDescription().find("Post-apocalyptic ",0,17) == -1:
						self.log(CvUnitInfo.getType()+" is unlocked concurrently or after its upgrade "+CvUnitUpgradeInfo.getType()+" "+str(iTechLoc)+"/"+str(iTechUpgradeLoc))
					if iTechObs < iTechUpgradeLoc and CvUnitInfo.getType().find("UNIT_NEANDERTHAL_",0,17) == -1 and CvUnitUpgradeInfo.getType().find("UNIT_NEANDERTHAL",0,16) == -1:
						self.log(CvUnitInfo.getType()+" is obsoleted before its upgrade being available "+CvUnitUpgradeInfo.getType()+" "+str(iTechObs)+"/"+str(iTechUpgradeLoc))
					if iTechObs > iTechUpgradeObs and CvUnitInfo.getProductionCost() > 0 and CvUnitInfo.getType().find("UNIT_NEANDERTHAL_",0,17) == -1 and CvUnitUpgradeInfo.getType().find("UNIT_NEANDERTHAL",0,16) == -1:
						self.log(CvUnitInfo.getType()+" is obsoleted after its upgrade obsoletion "+CvUnitUpgradeInfo.getType()+" "+str(iTechObs)+"/"+str(iTechUpgradeObs))

	#Unit - check unit bonus requirements
	def checkUnitBonusRequirements(self):
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			iTechLoc = self.HF.checkUnitTechRequirementLocation(CvUnitInfo)[0]

			#<BonusType>BONUS_X
			iBonus = CvUnitInfo.getPrereqAndBonus()
			if iBonus != -1:
				bonusTechLoc = self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2]
				if bonusTechLoc > iTechLoc:
					self.log(CvUnitInfo.getType()+" - Singular AND bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<PrereqBonuses>
			aBonusTechLocList = []
			for iBonus in CvUnitInfo.getPrereqOrBonuses():
				aBonusTechLocList.append(self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvUnitInfo.getType()+" - Earliest OR bonus prereq late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#<TrainCondition>
			aBonusGOMReqList = []
			for i in range(2):
				aBonusGOMReqList.append([])
			self.HF.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BONUS, aBonusGOMReqList)

			#Analyze GOM AND Bonus reqs
			aBonusTechLocList = []
			for iBonus in xrange(len(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aBonusTechLocList.append(self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBonus]))[2])
			if len(aBonusTechLocList) > 0 and max(aBonusTechLocList) > iTechLoc:
				self.log(CvUnitInfo.getType()+" - GOM AND bonus requirements are late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Bonus reqs
			aBonusTechLocList = []
			for iBonus in xrange(len(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aBonusTechLocList.append(self.HF.checkBonusTechRequirementLocation(GC.getBonusInfo(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBonus]))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvUnitInfo.getType()+" - GOM OR bonus requirements are late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

	#Unit - check building requirements
	def checkUnitRequirements(self):
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			iTechLoc = self.HF.checkUnitTechRequirementLocation(CvUnitInfo)[0]
			iTechObs = self.HF.checkUnitTechObsoletionLocation(CvUnitInfo)[0]

			#<PrereqAndBuildings> - require all buildings in list
			aBuildingNameList = []
			aBuildingTechLocList = []
			aBuildingTechObsList = []
			for iBuilding in xrange(CvUnitInfo.getNumPrereqAndBuildings()):
				aBuildingNameList.append(GC.getBuildingInfo(CvUnitInfo.getPrereqAndBuilding(iBuilding)).getType())
				aBuildingTechLocList.append(self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(CvUnitInfo.getPrereqAndBuilding(iBuilding)))[0])
				aBuildingTechObsList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(CvUnitInfo.getPrereqAndBuilding(iBuilding)))[0])
			if len(aBuildingTechLocList) > 0 and max(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" is unlocked before its AND building requirements "+str(aBuildingTechLocList)+" "+str(iTechLoc))
			if len(aBuildingTechObsList) > 0 and min(aBuildingTechObsList) < iTechObs:
				self.log(CvUnitInfo.getType()+" is obsoleted after its AND building requirements "+str(aBuildingTechObsList)+" "+str(iTechObs))

			#<PrereqOrBuildings> - require one building in list
			aBuildingNameList = []
			aBuildingTechLocList = []
			aBuildingTechObsList = []
			for iBuilding in xrange(CvUnitInfo.getPrereqOrBuildingsNum()):
				aBuildingNameList.append(GC.getBuildingInfo(CvUnitInfo.getPrereqOrBuilding(iBuilding)).getType())
				aBuildingTechLocList.append(self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(CvUnitInfo.getPrereqOrBuilding(iBuilding)))[0])
				aBuildingTechObsList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(CvUnitInfo.getPrereqOrBuilding(iBuilding)))[0])
			if len(aBuildingTechLocList) > 0 and min(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" is unlocked before its OR building requirement "+str(aBuildingTechLocList)+" "+str(iTechLoc))
			if len(aBuildingTechObsList) > 0 and max(aBuildingTechObsList) < iTechObs:
				self.log(CvUnitInfo.getType()+" is obsoleted after its OR building requirement "+str(aBuildingTechObsList)+" "+str(iTechObs))

			#<TrainCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.HF.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aBuildingNameList = []
			aBuildingTechLocList = []
			aBuildingTechObsList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aBuildingNameList.append(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]).getType())
				aBuildingTechLocList.append(self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]))[0])
				aBuildingTechObsList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]))[0])
			if len(aBuildingTechLocList) > 0 and max(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" is unlocked before its GOM AND building requirements "+str(aBuildingTechLocList)+" "+str(iTechLoc))
			if len(aBuildingTechObsList) > 0 and min(aBuildingTechObsList) < iTechObs:
				self.log(CvUnitInfo.getType()+" is obsoleted after its GOM AND building requirements "+str(aBuildingTechObsList)+" "+str(iTechObs))

			#Analyze GOM OR Building reqs
			aBuildingNameList = []
			aBuildingTechLocList = []
			aBuildingTechObsList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aBuildingNameList.append(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]).getType())
				aBuildingTechLocList.append(self.HF.checkBuildingTechRequirements(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]))[0])
				aBuildingTechObsList.append(self.HF.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]))[0])
			if len(aBuildingTechLocList) > 0 and min(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" is unlocked before its GOM OR building requirement "+str(aBuildingTechLocList)+" "+str(iTechLoc))
			if len(aBuildingTechObsList) > 0 and max(aBuildingTechObsList) < iTechObs:
				self.log(CvUnitInfo.getType()+" is obsoleted after its GOM OR building requirements "+str(aBuildingTechObsList)+" "+str(iTechObs))

	#Unit - check building requirement replacements
	def checkUnitRequirementsReplacements(self):
		aSpecialBuildingsList = [GC.getInfoTypeForString("BUILDING_POLLUTION_BLACKENEDSKIES"), GC.getInfoTypeForString("BUILDING_GAMBLING_BAN"), GC.getInfoTypeForString("BUILDING_ALCOCHOL_PROHIBITION"), GC.getInfoTypeForString("BUILDING_DRUG_PROHIBITION"), GC.getInfoTypeForString("BUILDING_PROSTITUTION_BAN")]
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)

			#<PrereqAndBuildings> - require all buildings in list
			aBuildingList = []
			for iBuilding in xrange(CvUnitInfo.getNumPrereqAndBuildings()):
				aBuildingList.append(CvUnitInfo.getPrereqAndBuilding(iBuilding))
			#Get unique buildings - distinct requirements
			aBuildingUniqueList = []
			for iBuilding in xrange(len(aBuildingList)):
				if aBuildingList[iBuilding] not in aBuildingUniqueList:
					aBuildingUniqueList.append(aBuildingList[iBuilding])
			#Get building replacements - all replacements are explicitly defined
			aBuildingReplacementList = []
			for iBuilding in xrange(len(aBuildingUniqueList)):
				CvBuildingInfo = GC.getBuildingInfo(aBuildingUniqueList[iBuilding])
				for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
					if CvBuildingInfo.getReplacementBuilding(iReplacement) not in aSpecialBuildingsList:
						aBuildingReplacementList.append(CvBuildingInfo.getReplacementBuilding(iReplacement))
			#Get unique replacements
			aReplacementUniqueList = []
			for iBuilding in xrange(len(aBuildingReplacementList)):
				if aBuildingReplacementList[iBuilding] not in aReplacementUniqueList:
					aReplacementUniqueList.append(aBuildingReplacementList[iBuilding])
			#Get replacements of requirements, that aren't listed as requirements
			aReplacedRequirements = []
			for iBuilding in xrange(len(aReplacementUniqueList)):
				if aReplacementUniqueList[iBuilding] not in aBuildingUniqueList:
					aReplacedRequirements.append(aReplacementUniqueList[iBuilding])
			#Get unique requirement replacements
			aReplacedUniqueRequirements = []
			for iBuilding in xrange(len(aReplacedRequirements)):
				if aReplacedRequirements[iBuilding] not in aReplacedUniqueRequirements:
					aReplacedUniqueRequirements.append(aReplacedRequirements[iBuilding])
			#Convert to Types
			aBuildingTypeList = []
			for iBuilding in xrange(len(aReplacedUniqueRequirements)):
				aBuildingTypeList.append(GC.getBuildingInfo(aReplacedUniqueRequirements[iBuilding]).getType())
			if len(aBuildingTypeList) > 0:
				self.log(CvUnitInfo.getType()+" requires (AND) "+str(aBuildingTypeList))

			#<PrereqOrBuildings> - require one building in list
			aBuildingList = []
			for iBuilding in xrange(CvUnitInfo.getPrereqOrBuildingsNum()):
				aBuildingList.append(CvUnitInfo.getPrereqOrBuilding(iBuilding))
			#Get unique buildings - distinct requirements
			aBuildingUniqueList = []
			for iBuilding in xrange(len(aBuildingList)):
				if aBuildingList[iBuilding] not in aBuildingUniqueList:
					aBuildingUniqueList.append(aBuildingList[iBuilding])
			#Get building replacements - all replacements are explicitly defined
			aBuildingReplacementList = []
			for iBuilding in xrange(len(aBuildingUniqueList)):
				CvBuildingInfo = GC.getBuildingInfo(aBuildingUniqueList[iBuilding])
				for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
					if CvBuildingInfo.getReplacementBuilding(iReplacement) not in aSpecialBuildingsList:
						aBuildingReplacementList.append(CvBuildingInfo.getReplacementBuilding(iReplacement))
			#Get unique replacements
			aReplacementUniqueList = []
			for iBuilding in xrange(len(aBuildingReplacementList)):
				if aBuildingReplacementList[iBuilding] not in aReplacementUniqueList:
					aReplacementUniqueList.append(aBuildingReplacementList[iBuilding])
			#Get replacements of requirements, that aren't listed as requirements
			aReplacedRequirements = []
			for iBuilding in xrange(len(aReplacementUniqueList)):
				if aReplacementUniqueList[iBuilding] not in aBuildingUniqueList:
					aReplacedRequirements.append(aReplacementUniqueList[iBuilding])
			#Get unique requirement replacements
			aReplacedUniqueRequirements = []
			for iBuilding in xrange(len(aReplacedRequirements)):
				if aReplacedRequirements[iBuilding] not in aReplacedUniqueRequirements:
					aReplacedUniqueRequirements.append(aReplacedRequirements[iBuilding])
			#Convert to Types
			aBuildingTypeList = []
			for iBuilding in xrange(len(aReplacedUniqueRequirements)):
				aBuildingTypeList.append(GC.getBuildingInfo(aReplacedUniqueRequirements[iBuilding]).getType())
			if len(aBuildingTypeList) > 0:
				self.log(CvUnitInfo.getType()+" requires (OR) "+str(aBuildingTypeList))

			#<TrainCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.HF.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aBuildingList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aBuildingList.append(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding])
			#Get unique buildings - distinct requirements
			aBuildingUniqueList = []
			for iBuilding in xrange(len(aBuildingList)):
				if aBuildingList[iBuilding] not in aBuildingUniqueList:
					aBuildingUniqueList.append(aBuildingList[iBuilding])
			#Get building replacements - all replacements are explicitly defined
			aBuildingReplacementList = []
			for iBuilding in xrange(len(aBuildingUniqueList)):
				CvBuildingInfo = GC.getBuildingInfo(aBuildingUniqueList[iBuilding])
				for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
					if CvBuildingInfo.getReplacementBuilding(iReplacement) not in aSpecialBuildingsList:
						aBuildingReplacementList.append(CvBuildingInfo.getReplacementBuilding(iReplacement))
			#Get unique replacements
			aReplacementUniqueList = []
			for iBuilding in xrange(len(aBuildingReplacementList)):
				if aBuildingReplacementList[iBuilding] not in aReplacementUniqueList:
					aReplacementUniqueList.append(aBuildingReplacementList[iBuilding])
			#Get replacements of requirements, that aren't listed as requirements
			aReplacedRequirements = []
			for iBuilding in xrange(len(aReplacementUniqueList)):
				if aReplacementUniqueList[iBuilding] not in aBuildingUniqueList:
					aReplacedRequirements.append(aReplacementUniqueList[iBuilding])
			#Get unique requirement replacements
			aReplacedUniqueRequirements = []
			for iBuilding in xrange(len(aReplacedRequirements)):
				if aReplacedRequirements[iBuilding] not in aReplacedUniqueRequirements:
					aReplacedUniqueRequirements.append(aReplacedRequirements[iBuilding])
			#Convert to Types
			aBuildingTypeList = []
			for iBuilding in xrange(len(aReplacedUniqueRequirements)):
				aBuildingTypeList.append(GC.getBuildingInfo(aReplacedUniqueRequirements[iBuilding]).getType())
			if len(aBuildingTypeList) > 0:
				self.log(CvUnitInfo.getType()+" requires (GOM AND) "+str(aBuildingTypeList))

			#Analyze GOM OR Building reqs
			aBuildingList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aBuildingList.append(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding])
			#Get unique buildings - distinct requirements
			aBuildingUniqueList = []
			for iBuilding in xrange(len(aBuildingList)):
				if aBuildingList[iBuilding] not in aBuildingUniqueList:
					aBuildingUniqueList.append(aBuildingList[iBuilding])
			#Get building replacements - all replacements are explicitly defined
			aBuildingReplacementList = []
			for iBuilding in xrange(len(aBuildingUniqueList)):
				CvBuildingInfo = GC.getBuildingInfo(aBuildingUniqueList[iBuilding])
				for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
					if CvBuildingInfo.getReplacementBuilding(iReplacement) not in aSpecialBuildingsList:
						aBuildingReplacementList.append(CvBuildingInfo.getReplacementBuilding(iReplacement))
			#Get unique replacements
			aReplacementUniqueList = []
			for iBuilding in xrange(len(aBuildingReplacementList)):
				if aBuildingReplacementList[iBuilding] not in aReplacementUniqueList:
					aReplacementUniqueList.append(aBuildingReplacementList[iBuilding])
			#Get replacements of requirements, that aren't listed as requirements
			aReplacedRequirements = []
			for iBuilding in xrange(len(aReplacementUniqueList)):
				if aReplacementUniqueList[iBuilding] not in aBuildingUniqueList:
					aReplacedRequirements.append(aReplacementUniqueList[iBuilding])
			#Get unique requirement replacements
			aReplacedUniqueRequirements = []
			for iBuilding in xrange(len(aReplacedRequirements)):
				if aReplacedRequirements[iBuilding] not in aReplacedUniqueRequirements:
					aReplacedUniqueRequirements.append(aReplacedRequirements[iBuilding])
			#Convert to Types
			aBuildingTypeList = []
			for iBuilding in xrange(len(aReplacedUniqueRequirements)):
				aBuildingTypeList.append(GC.getBuildingInfo(aReplacedUniqueRequirements[iBuilding]).getType())
			if len(aBuildingTypeList) > 0:
				self.log(CvUnitInfo.getType()+" requires (GOM OR) "+str(aBuildingTypeList))

	#Checks if unit default AI is listed in unit AIs
	def checkUnitAis(self):
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)

			iDefaultAI = CvUnitInfo.getDefaultUnitAIType()
			if not CvUnitInfo.getUnitAIType(iDefaultAI):
				self.log(CvUnitInfo.getType()+" doesn't have UnitAi of DefaultAI")


	#Bonus - check improvement productivity
	def checkBonusImprovementProductivity(self):
		for iBonus in xrange(GC.getNumBonusInfos()):
			CvBonusInfo = GC.getBonusInfo(iBonus)
			#Check total productivity: from resource, improvement, improvement+resource coupling, and with all tech boosts.
			if CvBonusInfo.getConstAppearance() > 0: # Only care about map resources
				for iImprovement in xrange(GC.getNumImprovementInfos()):
					CvImprovementInfo = GC.getImprovementInfo(iImprovement)
					if CvImprovementInfo.getImprovementUpgrade() != -1 or CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes() > 0 or CvImprovementInfo.getImprovementPillage() != -1: # Only care about improvements, that can upgrade or downgrade.
						if CvImprovementInfo.isImprovementBonusTrade(iBonus) and not CvImprovementInfo.isActsAsCity(): # Only care about improvements, that can improve bonus
							#Zero out data
							aBonusYield = [0]*YieldTypes.NUM_YIELD_TYPES
							aImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
							aTechImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
							aBonusImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
							aFinalYield = [0]*YieldTypes.NUM_YIELD_TYPES

							aImprovementUpgradeYield = [0]*YieldTypes.NUM_YIELD_TYPES
							aBonusImprovementUpgradeYield = [0]*YieldTypes.NUM_YIELD_TYPES
							aFinalImpUpgradeYield = [0]*YieldTypes.NUM_YIELD_TYPES
							for iYield in xrange(YieldTypes.NUM_YIELD_TYPES): # Food, Production, Commerce
								aBonusYield[iYield] = CvBonusInfo.getYieldChange(iYield) # Bonus yields
								aImprovementYield[iYield] = CvImprovementInfo.getYieldChange(iYield) # Improvement yields
								aBonusImprovementYield[iYield] = CvImprovementInfo.getImprovementBonusYield(iBonus, iYield) # Bonus-Improvement coupling yields

								for iTech in xrange(GC.getNumTechInfos()):  # Find techs, that boost base improvement
									if CvImprovementInfo.getTechYieldChanges(iTech, iYield) != 0:
										aTechImprovementYield[iYield] += CvImprovementInfo.getTechYieldChanges(iTech, iYield)

								aFinalYield[iYield] = aBonusYield[iYield] + aImprovementYield[iYield] + aTechImprovementYield[iYield] + aBonusImprovementYield[iYield]

								CvImprovementUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getImprovementUpgrade())
								if CvImprovementInfo.getImprovementUpgrade() != -1 and CvImprovementUpgradeInfo.isImprovementBonusTrade(iBonus):
									aImprovementUpgradeYield[iYield] = CvImprovementUpgradeInfo.getYieldChange(iYield)
									aBonusImprovementUpgradeYield[iYield] = CvImprovementUpgradeInfo.getImprovementBonusYield(iBonus, iYield)
									aFinalImpUpgradeYield[iYield] = aBonusYield[iYield] + aImprovementUpgradeYield[iYield] + aBonusImprovementUpgradeYield[iYield]

							# Upgrades
							if (CvImprovementInfo.getImprovementUpgrade() != -1 and CvImprovementUpgradeInfo.isImprovementBonusTrade(iBonus)) and (aFinalImpUpgradeYield[0] < aFinalYield[0] or aFinalImpUpgradeYield[1] < aFinalYield[1] or aFinalImpUpgradeYield[2] < aFinalYield[2]):
								self.log(CvImprovementInfo.getType()+" with "+CvBonusInfo.getType()+": Yield -> "+str(aFinalYield)+" upgrade: "+CvImprovementUpgradeInfo.getType()+": Yield -> "+str((aFinalImpUpgradeYield, (aFinalImpUpgradeYield[0]-aFinalYield[0], aFinalImpUpgradeYield[1]-aFinalYield[1], aFinalImpUpgradeYield[2]-aFinalYield[2]))))

							# Alt upgrades
							for iImprovementUpgrade in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
								CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementUpgrade))
								if CvImprovementAltUpgradeInfo.isImprovementBonusTrade(iBonus):
									aImprovementAltUpgradeYield = [0]*YieldTypes.NUM_YIELD_TYPES
									aBonusImprovementAltUpgradeYield = [0]*YieldTypes.NUM_YIELD_TYPES
									aFinalImpAltUpgradeYield = [0]*YieldTypes.NUM_YIELD_TYPES
									for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
										aImprovementAltUpgradeYield[iYield] = CvImprovementAltUpgradeInfo.getYieldChange(iYield)
										aBonusImprovementAltUpgradeYield[iYield] = CvImprovementAltUpgradeInfo.getImprovementBonusYield(iBonus, iYield)
										aFinalImpAltUpgradeYield[iYield] = aBonusYield[iYield] + aImprovementAltUpgradeYield[iYield] + aBonusImprovementAltUpgradeYield[iYield]
									if (aFinalImpAltUpgradeYield[0] < aFinalYield[0] or aFinalImpAltUpgradeYield[1] < aFinalYield[1] or aFinalImpAltUpgradeYield[2] < aFinalYield[2]):
										self.log(CvImprovementInfo.getType()+" with "+CvBonusInfo.getType()+": Yield -> "+str(aFinalYield)+" Alt upgrade: "+CvImprovementAltUpgradeInfo.getType()+": Yield -> "+str((aFinalImpAltUpgradeYield, (aFinalImpAltUpgradeYield[0]-aFinalYield[0], aFinalImpAltUpgradeYield[1]-aFinalYield[1], aFinalImpAltUpgradeYield[2]-aFinalYield[2]))))

	#Bonus - check potential bonus producer replacements
	def checkBonusProducerReplacements(self):
		for iBonus in xrange(GC.getNumBonusInfos()):
			CvBonusInfo = GC.getBonusInfo(iBonus)

			#We care about manufactured class of resources - others are map ones or produced by wonder.
			if CvBonusInfo.getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_MANUFACTURED"):
				for iBuilding in xrange(GC.getNumBuildingInfos()):
					CvBuildingInfo = GC.getBuildingInfo(iBuilding)

					#Earth bonus producers should always have replacements, if its regular manufactured one, ignore wonders in this case
					if GC.getInfoTypeForString("MAPCATEGORY_EARTH") in CvBuildingInfo.getMapCategories() and CvBuildingInfo.getType().find("_NATURAL_WONDER_") == -1 and not isNationalWonder(iBuilding) and not isWorldWonder(iBuilding):
						if iBonus in CvBuildingInfo.getFreeBonuses() and CvBuildingInfo.getNumReplacedBuilding() == 0 and CvBuildingInfo.getNumReplacementBuilding() == 0:
							self.log(CvBonusInfo.getType()+" producer "+CvBuildingInfo.getType()+" is standalone")

	#Civic - check if civic yield bonus for improvement is carried into its upgrade
	def checkCivicImprovementReplacements(self):
		for iCivic in xrange(GC.getNumCivicInfos()):
			CvCivicInfo = GC.getCivicInfo(iCivic)

			aImprovementList = []
			aImprovementUpgrades = []
			aImprovementUniqueUpgrades = []
			aImprovementsList = []

			if CvCivicInfo.isAnyImprovementYieldChange():
				#Get <ImprovementYieldChanges>
				for iImprovement in xrange(GC.getNumImprovementInfos()):
					if CvCivicInfo.getImprovementYieldChanges(iImprovement, 0) != 0 or CvCivicInfo.getImprovementYieldChanges(iImprovement, 1) != 0 or CvCivicInfo.getImprovementYieldChanges(iImprovement, 2) != 0:
						aImprovementList.append(iImprovement)

				#Analyze list of improvements
				for i in xrange(len(aImprovementList)):
					CvImprovementInfo = GC.getImprovementInfo(aImprovementList[i])
					if CvImprovementInfo.getImprovementUpgrade() != -1:
						iImprovementUpgrade = CvImprovementInfo.getImprovementUpgrade()
						szUpgradedImprovement = GC.getImprovementInfo(iImprovementUpgrade).getType()
						aImprovementUpgrades.append(iImprovementUpgrade)
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							if CvCivicInfo.getImprovementYieldChanges(aImprovementList[i], iYield) > CvCivicInfo.getImprovementYieldChanges(iImprovementUpgrade, iYield):
								self.log(CvCivicInfo.getType()+" "+CvImprovementInfo.getType()+" -> "+szUpgradedImprovement+" yield degradation")
					for iImprovementReplacement in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
						iImprovementUpgrade = CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementReplacement)
						szUpgradedImprovement = GC.getImprovementInfo(iImprovementUpgrade).getType()
						aImprovementUpgrades.append(iImprovementUpgrade)
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							if CvCivicInfo.getImprovementYieldChanges(aImprovementList[i], iYield) > CvCivicInfo.getImprovementYieldChanges(iImprovementUpgrade, iYield):
								self.log(CvCivicInfo.getType()+" "+CvImprovementInfo.getType()+" alt -> "+szUpgradedImprovement+" yield degradation")

				#We want nonrepeating list
				for i in xrange(len(aImprovementUpgrades)):
					if aImprovementUpgrades[i] not in aImprovementUniqueUpgrades:
						aImprovementUniqueUpgrades.append(aImprovementUpgrades[i])

				#If improvement is listed, then remove it
				for i in xrange(len(aImprovementList)):
					if aImprovementList[i] in aImprovementUniqueUpgrades:
						aImprovementUniqueUpgrades.remove(aImprovementList[i])

				#Get names
				for i in xrange(len(aImprovementUniqueUpgrades)):
					aImprovementsList.append(GC.getImprovementInfo(aImprovementUniqueUpgrades[i]).getType())

				if len(aImprovementsList) > 0:
					self.log(CvCivicInfo.getType()+" should have improvement upgrades for ImprovementYieldChanges "+str(aImprovementsList))

	#Trait - check if trait yield bonus for improvement is carried into its upgrade
	def checkTraitImprovementReplacements(self):
		for iTrait in xrange(GC.getNumTraitInfos()):
			CvTraitInfo = GC.getTraitInfo(iTrait)

			aImprovementList = []
			aImprovementUpgrades = []
			aImprovementUniqueUpgrades = []
			aImprovementsList = []

			#Get <ImprovementYieldChange>
			for iImprovement in xrange(GC.getNumImprovementInfos()):
				if CvTraitInfo.getImprovementYieldChange(iImprovement, 0) != 0 or CvTraitInfo.getImprovementYieldChange(iImprovement, 1) != 0 or CvTraitInfo.getImprovementYieldChange(iImprovement, 2) != 0:
					aImprovementList.append(iImprovement)

			if len(aImprovementList) > 0:
				#Analyze list of improvements
				for i in xrange(len(aImprovementList)):
					CvImprovementInfo = GC.getImprovementInfo(aImprovementList[i])
					if CvImprovementInfo.getImprovementUpgrade() != -1:
						iImprovementUpgrade = CvImprovementInfo.getImprovementUpgrade()
						szUpgradedImprovement = GC.getImprovementInfo(iImprovementUpgrade).getType()
						aImprovementUpgrades.append(iImprovementUpgrade)
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							if CvTraitInfo.getImprovementYieldChange(aImprovementList[i], iYield) > CvTraitInfo.getImprovementYieldChange(iImprovementUpgrade, iYield):
								self.log(CvTraitInfo.getType()+" "+CvImprovementInfo.getType()+" -> "+szUpgradedImprovement+" yield degradation")
					for iImprovementReplacement in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
						iImprovementUpgrade = CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementReplacement)
						szUpgradedImprovement = GC.getImprovementInfo(iImprovementUpgrade).getType()
						aImprovementUpgrades.append(iImprovementUpgrade)
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							if CvTraitInfo.getImprovementYieldChange(aImprovementList[i], iYield) > CvTraitInfo.getImprovementYieldChange(iImprovementUpgrade, iYield):
								self.log(CvTraitInfo.getType()+" "+CvImprovementInfo.getType()+" alt -> "+szUpgradedImprovement+" yield degradation")

				#We want nonrepeating list
				for i in xrange(len(aImprovementUpgrades)):
					if aImprovementUpgrades[i] not in aImprovementUniqueUpgrades:
						aImprovementUniqueUpgrades.append(aImprovementUpgrades[i])

				#If improvement is listed, then remove it
				for i in xrange(len(aImprovementList)):
					if aImprovementList[i] in aImprovementUniqueUpgrades:
						aImprovementUniqueUpgrades.remove(aImprovementList[i])

				#Get names
				for i in xrange(len(aImprovementUniqueUpgrades)):
					aImprovementsList.append(GC.getImprovementInfo(aImprovementUniqueUpgrades[i]).getType())

				if len(aImprovementsList) > 0:
					self.log(CvTraitInfo.getType()+" should have improvement upgrades for ImprovementYieldChanges "+str(aImprovementsList))

	#Checks if earliest worker is unlocked when build is available
	def checkUnitBuildTechAligment(self):
		for iBuild in xrange(GC.getNumBuildInfos()):
			CvBuildInfo = GC.getBuildInfo(iBuild)
			iBuildTechID = self.HF.checkBuildTechRequirementLocation(CvBuildInfo)[2] #Tech ID
			aUnitTechID = []
			aUnitType = []

			for iUnit in xrange(GC.getNumUnitInfos()):
				CvUnitInfo = GC.getUnitInfo(iUnit)
				if CvUnitInfo.hasBuild(BuildTypes(iBuild)) and self.HF.checkUnitTechRequirementLocation(CvUnitInfo)[2] != 0: #Multiple Units can create same Build
					aUnitTechID.append(self.HF.checkUnitTechRequirementLocation(CvUnitInfo)[2]) #Tech ID
					aUnitType.append(CvUnitInfo.getType())

			if len(aUnitTechID) > 0 and iBuildTechID != 0 and iBuildTechID < min(aUnitTechID): #Build tech position is equal to earliest Unit tech position
				self.log(aUnitType[aUnitTechID.index(min(aUnitTechID))]+" and "+CvBuildInfo.getType()+" have different techs "+str(min(aUnitTechID))+"/"+str(iBuildTechID))

	#Checks if improvement and build have same tech unlock
	def checkBuildImprovementTechAligment(self):
		for iImprovement in xrange(GC.getNumImprovementInfos()):
			CvImprovementInfo = GC.getImprovementInfo(iImprovement)
			iImprovementTechID = self.HF.checkImprovementTechRequirementLocation(CvImprovementInfo)[2] #Tech ID
			aBuildTechID = []
			aBuildType = []

			for iBuild in xrange(GC.getNumBuildInfos()):
				CvBuildInfo = GC.getBuildInfo(iBuild)
				if CvBuildInfo.getImprovement() == iImprovement: #Multiple builds can create same improvement (kill and no kill variants), some builds have no improvements
					aBuildTechID.append(self.HF.checkBuildTechRequirementLocation(CvBuildInfo)[2]) #Tech ID
					aBuildType.append(CvBuildInfo.getType())

			if len(aBuildTechID) > 0 and iImprovementTechID != min(aBuildTechID): #Improvement tech position is equal to earliest build tech position
				self.log(aBuildType[aBuildTechID.index(min(aBuildTechID))]+" and "+CvImprovementInfo.getType()+" have different techs "+str(min(aBuildTechID))+"/"+str(iImprovementTechID))

	#Improvement - Checks if earliest valid improvement isn't unlocked after resource tech enable
	def checkImprovementResourceTechUnlocks(self):
		for iBonus in xrange(GC.getNumBonusInfos()):
			CvBonusInfo = GC.getBonusInfo(iBonus)
			if CvBonusInfo.getConstAppearance() > 0: # Only care about map resources
				iBonusTechLoc = self.HF.checkBonusTechRequirementLocation(CvBonusInfo)[2] #Tech Enable XGrid
				iBonusTechID = self.HF.checkBonusTechRequirementLocation(CvBonusInfo)[5] #Tech Enable ID
				aImprovementTechLoc = []
				aImprovementTechID = []
				aImprovementType = []

				for iImprovement in xrange(GC.getNumImprovementInfos()):
					CvImprovementInfo = GC.getImprovementInfo(iImprovement)
					if CvImprovementInfo.isImprovementBonusMakesValid(iBonus) or CvImprovementInfo.isImprovementBonusTrade(iBonus):
						aImprovementTechLoc.append(self.HF.checkImprovementTechRequirementLocation(CvImprovementInfo)[0])
						aImprovementTechID.append(self.HF.checkImprovementTechRequirementLocation(CvImprovementInfo)[2])
						aImprovementType.append(CvImprovementInfo.getType())

				if iBonusTechLoc < min(aImprovementTechLoc) or (iBonusTechLoc == min(aImprovementTechLoc) and iBonusTechID != min(aImprovementTechID)):
					self.log(CvBonusInfo.getType()+" tech enable is before, or on same column but different tech with earliest improvement: "+aImprovementType[aImprovementTechLoc.index(min(aImprovementTechLoc))])

	#Improvement - Yield boosts should be between improvement unlock and upgrade
	def checkImprovementTechYieldBoostLocation(self):
		for iImprovement in xrange(GC.getNumImprovementInfos()):
			CvImprovementInfo = GC.getImprovementInfo(iImprovement)
			iTechLoc = self.HF.checkImprovementTechRequirementLocation(CvImprovementInfo)[0]

			if CvImprovementInfo.getImprovementUpgrade() != -1 or CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes() > 0 or CvImprovementInfo.getImprovementPillage() != -1: #Only those, that can upgrade, or are top of upgrade chain
				aTechBoost = []
				CvImprovementUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getImprovementUpgrade())
				iImpUpgradeTechLoc = 0
				if CvImprovementUpgradeInfo != None: # Main upgrade
					iImpUpgradeTechLoc = self.HF.checkImprovementTechRequirementLocation(CvImprovementUpgradeInfo)[0]
				for iTech in xrange(GC.getNumTechInfos()):  # Find techs, that boost base improvement
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if CvImprovementInfo.getTechYieldChanges(iTech, iYield) != 0:
							aTechBoost.append(GC.getTechInfo(iTech).getGridX())
				if aTechBoost and iTechLoc >= min(aTechBoost):
					self.log(CvImprovementInfo.getType()+" Xgrid: "+str(iTechLoc)+" Tech boosts location: "+str(aTechBoost))
				if iImpUpgradeTechLoc and aTechBoost and iImpUpgradeTechLoc <= max(aTechBoost):
					self.log(CvImprovementInfo.getType()+" Xgrid: "+str(iTechLoc)+" Tech boosts location: "+str(aTechBoost)+" Upgrade: "+CvImprovementUpgradeInfo.getType()+": "+str(iImpUpgradeTechLoc))

				#Alt upgrades
				for iImprovementUpgrade in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementUpgrade))
					iImpAltUpgradeTechLoc = 0
					if CvImprovementAltUpgradeInfo != None: # Alt upgrade
						iImpAltUpgradeTechLoc = self.HF.checkImprovementTechRequirementLocation(CvImprovementUpgradeInfo)[0]
						if iImpAltUpgradeTechLoc and aTechBoost and iImpAltUpgradeTechLoc <= max(aTechBoost):
							self.log(CvImprovementInfo.getType()+" Xgrid: "+str(iTechLoc)+" Tech boosts location: "+str(aTechBoost)+" Alt Upgrade: "+CvImprovementAltUpgradeInfo.getType()+": "+str(iImpAltUpgradeTechLoc))

	#Improvement - Base + tech improvement yields compared to upgraded improvement
	def checkImprovementYieldValues(self):
		for iImprovement in xrange(GC.getNumImprovementInfos()):
			CvImprovementInfo = GC.getImprovementInfo(iImprovement)
			if CvImprovementInfo.getImprovementUpgrade() != -1 or CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes() > 0 or CvImprovementInfo.getImprovementPillage() != -1: #Only those, that can upgrade, or are top of upgrade chain
				iTechLoc = self.HF.checkImprovementTechRequirementLocation(CvImprovementInfo)[0]
				#Improvement yield with all techs
				aBaseImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
				aBaseUpgradeImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
				aTechImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
				aTotalImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES

				CvImprovementUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getImprovementUpgrade())
				for iYield in xrange(YieldTypes.NUM_YIELD_TYPES): # Food, Production, Commerce
					aBaseImprovementYield[iYield] = CvImprovementInfo.getYieldChange(iYield) # Improvement yields
					for iTech in xrange(GC.getNumTechInfos()):  # Find techs, that boost base improvement
						if CvImprovementInfo.getTechYieldChanges(iTech, iYield) != 0:
							aTechImprovementYield[iYield] += CvImprovementInfo.getTechYieldChanges(iTech, iYield)
					aTotalImprovementYield[iYield] = aBaseImprovementYield[iYield] + aTechImprovementYield[iYield]
					if CvImprovementUpgradeInfo != None: # Main upgrade
						aBaseUpgradeImprovementYield[iYield] = CvImprovementUpgradeInfo.getYieldChange(iYield)

				if CvImprovementUpgradeInfo != None and (aTotalImprovementYield[0] > aBaseUpgradeImprovementYield[0] or aTotalImprovementYield[1] > aBaseUpgradeImprovementYield[1] or aTotalImprovementYield[2] > aBaseUpgradeImprovementYield[2]):
					self.log(CvImprovementInfo.getType()+" Total Yield: "+str(aTotalImprovementYield)+", "+CvImprovementUpgradeInfo.getType()+" Upgrade Yield: "+str(aBaseUpgradeImprovementYield))

				for iImprovementUpgrade in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					aBaseAltUpgradeImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
					CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementUpgrade))
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						aBaseAltUpgradeImprovementYield[iYield] = CvImprovementAltUpgradeInfo.getYieldChange(iYield)
					if aTotalImprovementYield[0] > aBaseAltUpgradeImprovementYield[0] or aTotalImprovementYield[1] > aBaseAltUpgradeImprovementYield[1] or aTotalImprovementYield[2] > aBaseAltUpgradeImprovementYield[2]:
						self.log(CvImprovementInfo.getType()+" Total Yield: "+str(aTotalImprovementYield)+", "+CvImprovementAltUpgradeInfo.getType()+" Alt Upgrade Yield: "+str(aBaseAltUpgradeImprovementYield))

	#Improvement - Checks if upgrade of improvement is more likely to spawn resource
	def checkImprovementBonusSpawnChance(self):
		for iImprovement in xrange(GC.getNumImprovementInfos()):
			CvImprovementInfo = GC.getImprovementInfo(iImprovement)
			iNoDiscovery = 999999

			if CvImprovementInfo.getImprovementUpgrade() != -1 or CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes() > 0: #Check improvements, that can upgrade
				aBaseBonusChance = [iNoDiscovery]*GC.getNumBonusInfos()
				for iBonus in xrange(GC.getNumBonusInfos()):
					if CvImprovementInfo.getImprovementBonusDiscoverRand(iBonus) != 0:
						aBaseBonusChance[iBonus] = CvImprovementInfo.getImprovementBonusDiscoverRand(iBonus)

				#Main upgrade
				CvImprovementUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getImprovementUpgrade())
				aUpgradeBonusChance = [iNoDiscovery]*GC.getNumBonusInfos()
				for jBonus in xrange(GC.getNumBonusInfos()):
					if CvImprovementUpgradeInfo.getImprovementBonusDiscoverRand(jBonus) != 0:
						aUpgradeBonusChance[jBonus] = CvImprovementUpgradeInfo.getImprovementBonusDiscoverRand(jBonus)

				#Compare rand chances of improvement and main upgrade
				for i in xrange(len(aBaseBonusChance)):
					if aBaseBonusChance[i] < aUpgradeBonusChance[i] or aBaseBonusChance[i] != 2*aUpgradeBonusChance[i] and aBaseBonusChance[i] != iNoDiscovery:
						self.log(CvImprovementInfo.getType()+" -> "+CvImprovementUpgradeInfo.getType()+" Bonus: "+GC.getBonusInfo(i).getType()+" Rand: "+str(aBaseBonusChance[i])+"->"+str(aUpgradeBonusChance[i]))

				#Alt upgrades
				for iImprovementUpgrade in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementUpgrade))
					aAltUpgradeBonusChance = [iNoDiscovery]*GC.getNumBonusInfos()
					for kBonus in xrange(GC.getNumBonusInfos()):
						if CvImprovementAltUpgradeInfo.getImprovementBonusDiscoverRand(kBonus) != 0:
							aAltUpgradeBonusChance[kBonus] = CvImprovementAltUpgradeInfo.getImprovementBonusDiscoverRand(kBonus)

					#Compare rand chances of improvement and alt upgrades
					for j in xrange(len(aBaseBonusChance)):
						if aBaseBonusChance[j] < aAltUpgradeBonusChance[j] or aBaseBonusChance[j] != 2*aAltUpgradeBonusChance[j] and aBaseBonusChance[j] != iNoDiscovery:
							self.log(CvImprovementInfo.getType()+" A-> "+CvImprovementAltUpgradeInfo.getType()+" Bonus: "+GC.getBonusInfo(j).getType()+" Rand: "+str(aBaseBonusChance[j])+"->"+str(aAltUpgradeBonusChance[j]))

	#Buildings - noncultural wonders, religious shrines and projects should have wonder movie tag, preferably in DDS format
	def checkBuildingWonderMovies(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()
			bCultural = 0
			if iSpecialBuilding != -1 and iSpecialBuilding == GC.getInfoTypeForString("SPECIALBUILDING_C2C_CULTURE"):
				bCultural = 1

			if isNationalWonder(iBuilding) or (isWorldWonder(iBuilding) and not bCultural) or CvBuildingInfo.getHolyCity() != -1:
				if CvBuildingInfo.getMovie() != None:
					self.log(CvBuildingInfo.getType()+" has movie in "+CvBuildingInfo.getMovie())
				else:
					self.log(CvBuildingInfo.getType()+" is missing a wonder movie!")

		for iProject in xrange(GC.getNumProjectInfos()):
			CvProjectInfo = GC.getProjectInfo(iProject)
			if not CvProjectInfo.isSpaceship():
				if CvProjectInfo.getMovieArtDef() != "":
					self.log(CvProjectInfo.getType()+" has movie art define tag: "+CvProjectInfo.getMovieArtDef())
				else:
					self.log(CvProjectInfo.getType()+" is missing a wonder movie!")

	#Building and unit - check if main tech requirement is more advanced or equal to tech types, main tech requirement determinates where to put icon in tech tree
	def checkTechTypes(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			aTechList = []
			aTechXY = []

			#Main tech requirement
			iTechMainReq = CvBuildingInfo.getPrereqAndTech()
			if iTechMainReq != -1:
				iTechMainLoc = GC.getTechInfo(iTechMainReq).getGridX()
				iTechMainRow = GC.getTechInfo(iTechMainReq).getGridY()
				aTechList.append(GC.getTechInfo(iTechMainReq).getType())
				aTechXY.append(100*iTechMainLoc+iTechMainRow)

			#Tech Type requirement
			aTechTypeLocList = []
			aTechTypeRowList = []
			for iTechType in CvBuildingInfo.getPrereqAndTechs():
				aTechList.append(GC.getTechInfo(iTechType).getType())
				aTechXY.append(100*GC.getTechInfo(iTechType).getGridX()+GC.getTechInfo(iTechType).getGridY())

			if len(aTechXY) > 1 and 100*iTechMainLoc+iTechMainRow != max(aTechXY):
				self.log(CvBuildingInfo.getType()+" Main tech isn't most advanced, switch it to "+str(aTechList[aTechXY.index(max(aTechXY))]))

		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			aTechList = []
			aTechXY = []

			#Main tech requirement
			iTechMainReq = CvUnitInfo.getPrereqAndTech()
			if iTechMainReq != -1:
				iTechMainLoc = GC.getTechInfo(iTechMainReq).getGridX()
				iTechMainRow = GC.getTechInfo(iTechMainReq).getGridY()
				aTechList.append(GC.getTechInfo(iTechMainReq).getType())
				aTechXY.append(100*iTechMainLoc+iTechMainRow)

			#Tech Type requirement
			aTechTypeLocList = []
			aTechTypeRowList = []
			for iTechType in CvUnitInfo.getPrereqAndTechs():
				aTechList.append(GC.getTechInfo(iTechType).getType())
				aTechXY.append(100*GC.getTechInfo(iTechType).getGridX()+GC.getTechInfo(iTechType).getGridY())

			if len(aTechXY) > 1 and 100*iTechMainLoc+iTechMainRow != max(aTechXY):
				self.log(CvUnitInfo.getType()+" Main tech isn't most advanced, switch it to "+str(aTechList[aTechXY.index(max(aTechXY))]))

	#Building - list buildings, that are stand-alone
	def listStandaloneBuildings(self):
		aSpecialReplacementsList = ["BUILDING_POLLUTION_BLACKENEDSKIES", "BUILDING_GAMBLING_BAN", "BUILDING_ALCOCHOL_PROHIBITION", "BUILDING_DRUG_PROHIBITION", "BUILDING_PROSTITUTION_BAN"]
		aSpecialBuildingList = [GC.getInfoTypeForString("SPECIALBUILDING_CORPORATION"), GC.getInfoTypeForString("SPECIALBUILDING_FOLKLORE_EXPLORATION")]
		#We are excluding bans from valid replacements, and some special building classes must be standalone

		#Get buildings and their requirements in 2D array
		aBuildings = [[0 for x in xrange(GC.getNumBuildingInfos())] for y in xrange(GC.getNumBuildingInfos())]
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
				aBuildings[iPrereqBuilding][iBuilding] = 1
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuildingRequirement)
				aBuildings[iPrereqBuilding][iBuilding] = 1
			for iPrereqBuilding, iNumRequired in CvBuildingInfo.getPrereqNumOfBuildings():
				aBuildings[iPrereqBuilding][iBuilding] = 1
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.HF.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuildingRequirement]
				aBuildings[iPrereqBuilding][iBuilding] = 1
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuildingRequirement]
				aBuildings[iPrereqBuilding][iBuilding] = 1

		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]
			iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()

			#Exclude Space buildings, National wonders, World wonders, Natural wonders, No cost buildings, continental culture, religious buildings, civic buildings, certain special buildings
			if GC.getInfoTypeForString("MAPCATEGORY_EARTH") in CvBuildingInfo.getMapCategories() and not isNationalWonder(iBuilding) and not isWorldWonder(iBuilding) and CvBuildingInfo.getType().find("_NATURAL_WONDER_") == -1 and CvBuildingInfo.getProductionCost() > 0 and CvBuildingInfo.getType().find("_CULTURE_LOCAL_") == -1 and CvBuildingInfo.getReligionType() == -1 and CvBuildingInfo.getPrereqReligion() == -1 and iSpecialBuilding not in aSpecialBuildingList:
				bRequiresCivic = False #Civic requiring buildings shouldn't be in upgrade chain
				for iCivic in xrange(GC.getNumCivicInfos()):
					if CvBuildingInfo.isPrereqAndCivics(iCivic):
						bRequiresCivic = True
						break
					if CvBuildingInfo.isPrereqOrCivics(iCivic):
						bRequiresCivic = True
						break

				if not bRequiresCivic:
					aReplacementList = []
					for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
						if GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(i)).getType() not in aSpecialReplacementsList:
							aReplacementList.append(CvBuildingInfo.getReplacementBuilding(i))
					aReplacedList = []
					for i in xrange(CvBuildingInfo.getNumReplacedBuilding()):
						if GC.getBuildingInfo(CvBuildingInfo.getReplacedBuilding(i)).getType() not in aSpecialReplacementsList:
							aReplacedList.append(CvBuildingInfo.getReplacedBuilding(i))

					iUsedBy = 0
					if max(aBuildings[iBuilding]) == 1:
						for i in xrange(len(aBuildings[iBuilding])):
							iUsedBy += aBuildings[iBuilding][i]

					#Standalone buildings, used by few buildings
					if len(aReplacementList) == 0 and len(aReplacedList) == 0 and iUsedBy <= 5:
						if CvBuildingInfo.getObsoleteTech() == -1:
							self.log(str(iTechLoc)+" tech column, "+CvBuildingInfo.getType()+" is stand-alone, required by "+str(iUsedBy)+" buildings")
						else:
							self.log(str(iTechLoc)+" tech column, "+CvBuildingInfo.getType()+" is stand-alone, required by "+str(iUsedBy)+" buildings, obsoletes at "+GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getType())

	#List how many buildings got unlocked/obsoleted
	def countUnlockedObsoletedBuildings(self):
		#Array length is amount of tech columns
		iTotalTechTreeLength = GC.getTechInfo(GC.getInfoTypeForString("TECH_FUTURE_TECH")).getGridX()
		iTotalActiveBuildings = 0
		aUnlockedBuildingsTechLoc = [0]*iTotalTechTreeLength
		aObsoletedBuildingsTechLoc = [0]*iTotalTechTreeLength

		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)

			#Increase count by 1 at index representing GridX of tech, that unlocks/obsoletes buildings
			aUnlockedBuildingsTechLoc[self.HF.checkBuildingTechRequirements(CvBuildingInfo)[0]] += 1
			if self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0] != 999: #If it obsoletes
				aObsoletedBuildingsTechLoc[self.HF.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]] += 1

		for i in xrange(iTotalTechTreeLength):
			iTotalActiveBuildings = iTotalActiveBuildings + aUnlockedBuildingsTechLoc[i] - aObsoletedBuildingsTechLoc[i]
			self.log("XGrid: "+str(i)+" Unlocked: "+str(aUnlockedBuildingsTechLoc[i])+" Obsoleted: "+str(aObsoletedBuildingsTechLoc[i])+" Available buildings: "+str(iTotalActiveBuildings))

	#List how many buildings provide bonuses
	def countBonusProducers(self):
		aBonusEnums = [0]*GC.getNumBonusInfos()
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)

			if GC.getInfoTypeForString("MAPCATEGORY_EARTH") in CvBuildingInfo.getMapCategories(): #Count only Earthly producers
				for iBonus, iNumFree in CvBuildingInfo.getFreeBonuses():
					aBonusEnums[iBonus] += 1

		for i in xrange(len(aBonusEnums)):
			if aBonusEnums[i] > 1: #If there are multiple providers
				self.log(str(aBonusEnums[i])+" "+GC.getBonusInfo(i).getType()+" providers")

	#List taxonomy buildings, that doesn't have all potential base folklore requirements
	def checkTaxonomyBuildings(self):
		aFolkloreBuildings = []
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			if CvBuildingInfo.getType().find("BUILDING_FOLKLORE") != -1:
				aFolkloreBuildings.append(iBuilding)

		aFolkloreUnits = []
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			if CvUnitInfo.getType().find("_SUBDUED") != -1:
				aFolkloreUnits.append(iUnit)

		for i in xrange(len(aFolkloreBuildings)):
			CvBuildingInfo = GC.getBuildingInfo(aFolkloreBuildings[i])
			if CvBuildingInfo.getSpecialBuildingType() == GC.getInfoTypeForString("SPECIALBUILDING_FOLKLORE_TAXONOMY"):
				for j in xrange(len(aFolkloreUnits)):
					CvUnitInfo = GC.getUnitInfo(aFolkloreUnits[j])
					if CvUnitInfo.getHasBuilding(aFolkloreBuildings[i]):
						for k in xrange(CvUnitInfo.getNumBuildings()):
							CvUnitBuilding = GC.getBuildingInfo(CvUnitInfo.getBuildings(k))
							if CvUnitBuilding.getSpecialBuildingType() != GC.getInfoTypeForString("SPECIALBUILDING_FOLKLORE_TAXONOMY") and CvUnitBuilding.getSpecialBuildingType() != GC.getInfoTypeForString("SPECIALBUILDING_FOLKLORE_EXPLORATION") and CvUnitBuilding.getType().find("BUILDING_FOLKLORE") != -1:
								if not CvBuildingInfo.isPrereqInCityBuilding(CvUnitInfo.getBuildings(k)) and not CvBuildingInfo.isPrereqOrBuilding(CvUnitInfo.getBuildings(k)):
									self.log(CvBuildingInfo.getType()+" could have as req: "+CvUnitBuilding.getType())