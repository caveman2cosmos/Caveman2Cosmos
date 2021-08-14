#
# TestCode - tab within DebugScreen
#
# Toffer - I intentionally made this very bare bones, hiding technical stuff inside DebugScreen.py
#	This so that python novices can utilize this file as a training/testing ground with minimal intimidation.
from CvPythonExtensions import *
GC = CyGlobalContext()
aTechID = []
aTechDesc = []

class TestCode:

	def __init__(self, pDebugScreen):
		self.main = pDebugScreen

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
		self.main.addTestCode(screen, self.checkBuildingUnlockObsoletion, "Buildings unlock/obsoletion", "Checks if building obsoletion doesn't happen within 10 columns of building unlock")
		self.main.addTestCode(screen, self.checkBuildingReplacementObsoletion, "Building obsoletion of replacements", "Checks when replacements are unlocked and obsoleted. Base -> Upgrade: Base tech obsoletion/Upgrade tech unlock, beelining might cause base building to go obsolete before replacement is available, difference of more than 10 columns is assumed safe. Replacing building shouldn't obsolete before replaced one")
		self.main.addTestCode(screen, self.checkBuildingImplicitReplacements, "Building - check implicit replacements", "Check if we have implicit replacements - All replacements must be explicitly defined even if building got obsoleted long ago")
		self.main.addTestCode(screen, self.checkBuildingReplacingProductivity, "Building - check replacement quality", "Check if building, that replaces earlier buildings is better in various metrics")
		self.main.addTestCode(screen, self.checkBuildingReplacingAvailability, "Building - check replacement availability", "Check if replaced buildings are affected by other buildings, civics or traits")
		self.main.addTestCode(screen, self.checkBuildingBonusRequirements, "Building bonus requirements", "Checks various bonus prereqs to check if they aren't unlocked after building")
		self.main.addTestCode(screen, self.checkBuildingBonusManufacturerTech, "Building earliest manufacturer on resource tech reveal", "Checks when earliest resource producer is unlocked")
		self.main.addTestCode(screen, self.checkBuildingRequirementCivics, "Building - requirement civic requirements", "Check if building requirements require civics")
		self.main.addTestCode(screen, self.checkBuildingCivicRequirements, "Building - civic requirements", "Checks if various civics aren't unlocked after building")
		self.main.addTestCode(screen, self.checkBuildingReligionRequirement, "Building religion requirement test", "Checks if tags requiring religion share same religion")
		self.main.addTestCode(screen, self.checkBuildingTags, "Building Tags", "Checks if commerce double time exists on wonders, that have relevant flat commerce change, if Commerce Change has relevant flat commerce changes, if hurry modifiers exist on unbuildable buildings, if GP unit references are paired with GP changes, or if freebonus amount is paired with bonus")
		self.main.addTestCode(screen, self.checkBuildingFreeReward, "Building obsoletion of free buildings", "Checks if free buildings - normally unbuildable - obsolete together with building, that gives it for free. Buildable free building shouldn't obsolete before building, that gives it for free")
		self.main.addTestCode(screen, self.checkBuildingTechMods, "Building tech changes and modifiers", "Checks if tech modifiers and changes occur within building lifetime")
		self.main.addTestCode(screen, self.checkBuildingBonusTags, "Building - check bonus tags", "Check if bonus tech reveal is after building obsoletion - those bonus tags affect buildings in various ways")
		self.main.addTestCode(screen, self.checkBuildingAffectingBuildings, "Building - check building tags", "Check if building affecting other building is within lifetime of each other")
		self.main.addTestCode(screen, self.checkBuildingCivicInfluences, "Building - check civic tags", "Check if building is available when civic is active")
		self.main.addTestCode(screen, self.checkUnitUpgrades, "Unit - check unit upgrades", "Checks unit upgrades")
		self.main.addTestCode(screen, self.checkUnitBonusRequirements, "Unit - check bonus requirements", "Checks bonus requirements of units")
		self.main.addTestCode(screen, self.checkUnitRequirements, "Unit - check building requirements", "Checks building requirements of units")
		self.main.addTestCode(screen, self.checkUnitRequirementsReplacements, "Unit - check building requirement replacements", "Checks if unit has building requirement, that gets replaced")
		self.main.addTestCode(screen, self.checkBonusImprovementProductivity, "Bonus - check improvement productivity", "Checks if improvement replacements productivity from bonus, improvement and bonus+improvement is higher compared to base improvement")
		self.main.addTestCode(screen, self.checkBonusProducerReplacements, "Bonus - check potential bonus producer replacements", "Checks replacements of manufactured bonus producers")
		self.main.addTestCode(screen, self.checkImprovementTechYieldBoostLocation, "Improvement - yield boost tech requirements", "Checks if yield boosts happen within tech unlock and replacement of improvements")
		self.main.addTestCode(screen, self.checkImprovementYieldValues, "Improvement - all techs boosts compared to upgrade", "Checks if improvement with all tech boosts isn't better than its upgrade")
		self.main.addTestCode(screen, self.checkBuildingWonderMovies, "Building movie wonder list", "Checks movies of noncultural wonders, religious shrines and projects movie location")
		self.main.addTestCode(screen, self.checkTechTypes, "Building and unit - Tech Types check", "Checks if buildings and units main tech is more advanced or equal to Tech Type")

	##### HIGHEST TECH REQUIREMENT LOCATION FINDER FUNCTIONS  #####

	#Building tech location
	def checkBuildingTechRequirements(self, CvBuildingInfo):
		#All required techs
		aTechList = []
		aTechGridXList = []
		aTechXY = []

		#Main tech requirement
		iTechMainReq = CvBuildingInfo.getPrereqAndTech()
		if iTechMainReq != -1:
			iTechMainLoc = GC.getTechInfo(iTechMainReq).getGridX()
			iTechMainRow = GC.getTechInfo(iTechMainReq).getGridY()
			aTechList.append(GC.getTechInfo(iTechMainReq).getType())
			aTechGridXList.append(iTechMainLoc)
			aTechXY.append(100*iTechMainLoc+iTechMainRow)
		else:
			iTechMainLoc = 0
			iTechMainRow = 0

		#Tech Type requirement
		aTechTypeLocList = []
		aTechTypeRowList = []
		for iTechType in CvBuildingInfo.getPrereqAndTechs():
			aTechTypeLocList.append(GC.getTechInfo(iTechType).getGridX())
			aTechTypeRowList.append(GC.getTechInfo(iTechType).getGridY())
			aTechList.append(GC.getTechInfo(iTechType).getType())
			aTechGridXList.append(GC.getTechInfo(iTechType).getGridX())
			aTechXY.append(100*GC.getTechInfo(iTechType).getGridX()+GC.getTechInfo(iTechType).getGridY())
		if len(aTechTypeLocList) > 0 and len(aTechTypeRowList) > 0:
			iTechTypeLoc = max(aTechTypeLocList)
			for iTechLoc in xrange(len(aTechTypeLocList)):
				if aTechTypeLocList[iTechLoc] == max(aTechTypeLocList):
					iTechTypeRow = aTechTypeRowList[iTechLoc]
		else:
			iTechTypeLoc = 0
			iTechTypeRow = 0

		#Tech requirement as defined in special building infos (core tech)
		iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()
		if iSpecialBuilding != -1:
			iTechSpecialReq = GC.getSpecialBuildingInfo(iSpecialBuilding).getTechPrereq()
			if iTechSpecialReq != -1:
				iTechSpecialLoc = GC.getTechInfo(iTechSpecialReq).getGridX()
				iTechSpecialRow = GC.getTechInfo(iTechSpecialReq).getGridY()
				aTechList.append(GC.getTechInfo(iTechSpecialReq).getType())
				aTechGridXList.append(iTechSpecialLoc)
				aTechXY.append(100*iTechSpecialLoc+iTechSpecialRow)
			elif iTechSpecialReq == -1:
				iTechSpecialLoc = 0
				iTechSpecialRow = 0
		else:
			iTechSpecialLoc = 0
			iTechSpecialRow = 0

		#Tech requirement derived from location of religion in tech tree
		iRelPrereq1 = CvBuildingInfo.getPrereqReligion()
		iRelPrereq2 = CvBuildingInfo.getReligionType()
		iRelPrereq3 = CvBuildingInfo.getPrereqStateReligion()
		if iRelPrereq1 != -1 or iRelPrereq2 != -1 or iRelPrereq3 != -1:
			iReligionBuilding = max(iRelPrereq1, iRelPrereq2, iRelPrereq3)
			if iReligionBuilding != -1:
				iTechReligionReq = GC.getReligionInfo(iReligionBuilding).getTechPrereq()
				if iTechReligionReq != -1:
					iTechReligionLoc = GC.getTechInfo(iTechReligionReq).getGridX()
					iTechReligionRow = GC.getTechInfo(iTechReligionReq).getGridY()
					aTechList.append(GC.getTechInfo(iTechReligionReq).getType())
					aTechGridXList.append(iTechReligionLoc)
					aTechXY.append(100*iTechReligionLoc+iTechReligionRow)
			elif iReligionBuilding == -1:
				iTechReligionLoc = 0
				iTechReligionRow = 0
		else:
			iTechReligionLoc = 0
			iTechReligionRow = 0

		#Myth/Story/Stories handling - X Require tech requirement is treated as one of tech requirements of building, assuming X Require is main building requirement.
		iTechAnimalLoc = 0
		iTechAnimalRow = 0
		for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
			iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
			if iPrereqBuilding == GC.getInfoTypeForString("BUILDING_ANIMAL_MYTH_REQUIRE") or \
				iPrereqBuilding == GC.getInfoTypeForString("BUILDING_ANIMAL_STORY_REQUIRE") or \
				iPrereqBuilding == GC.getInfoTypeForString("BUILDING_ANIMAL_STORIES_REQUIRE"):
				iTechAnimalLoc = GC.getTechInfo(GC.getBuildingInfo(iPrereqBuilding).getPrereqAndTech()).getGridX()
				iTechAnimalRow = GC.getTechInfo(GC.getBuildingInfo(iPrereqBuilding).getPrereqAndTech()).getGridY()
				aTechList.append(GC.getTechInfo(GC.getBuildingInfo(iPrereqBuilding).getPrereqAndTech()).getType())
				aTechGridXList.append(iTechAnimalLoc)
				aTechXY.append(100*iTechAnimalLoc+iTechAnimalRow)

		#Tech GOM requirements
		aTechGOMReqList = []
		aTechGOMAndLocList = []
		aTechGOMAndRowList = []
		aTechGOMOrLocList = []
		aTechGOMOrRowList = []
		aTechGOMOrTypeList = []
		for i in range(2):
			aTechGOMReqList.append([])
		self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_TECH, aTechGOMReqList)

		#Extract GOM AND requirements
		for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
			aTechGOMAndLocList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getGridX())
			aTechGOMAndRowList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getGridY())
			aTechList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getType())
			aTechGridXList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getGridX())
			aTechXY.append(100*GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getGridX()+GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getGridY())
		if len(aTechGOMAndLocList) > 0 and len(aTechGOMAndRowList) > 0:
			iTechGOMAndLoc = max(aTechGOMAndLocList)
			for iTechLoc in xrange(len(aTechGOMAndLocList)):
				if aTechGOMAndLocList[iTechLoc] == max(aTechGOMAndLocList):
					iTechGOMAndRow = aTechGOMAndRowList[iTechLoc]
		else:
			iTechGOMAndLoc = 0
			iTechGOMAndRow = 0

		#Extract GOM OR requirements
		for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
			aTechGOMOrLocList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR][iTech]).getGridX())
			aTechGOMOrRowList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR][iTech]).getGridY())
			aTechGOMOrTypeList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR][iTech]).getType())
		if len(aTechGOMOrLocList) > 0 and len(aTechGOMOrRowList) > 0:
			iTechGOMOrLoc = min(aTechGOMOrLocList)
			aTechGridXList.append(iTechGOMOrLoc)
			for iTechLoc in xrange(len(aTechGOMOrLocList)):
				if aTechGOMOrLocList[iTechLoc] == min(aTechGOMOrLocList):
					iTechGOMOrRow = aTechGOMOrRowList[iTechLoc]
					sTechGOMOrType = aTechGOMOrTypeList[iTechLoc]
					aTechList.append(sTechGOMOrType)
					aTechXY.append(100*iTechGOMOrLoc+iTechGOMOrRow)
		else:
			iTechGOMOrLoc = 0
			iTechGOMOrRow = 0

		#Pick most advanced column
		iTechLoc = max(iTechMainLoc, iTechTypeLoc, iTechSpecialLoc, iTechReligionLoc, iTechAnimalLoc, iTechGOMAndLoc, iTechGOMOrLoc)
		if iTechLoc == iTechMainLoc:
			iTechRow = iTechMainRow
		elif iTechLoc == iTechTypeLoc:
			iTechRow = iTechTypeRow
		elif iTechLoc == iTechSpecialLoc:
			iTechRow = iTechSpecialRow
		elif iTechLoc == iTechReligionLoc:
			iTechRow = iTechReligionRow
		elif iTechLoc == iTechAnimalLoc:
			iTechRow = iTechAnimalRow
		elif iTechLoc == iTechGOMAndLoc:
			iTechRow = iTechGOMAndRow
		elif iTechLoc == iTechGOMOrLoc:
			iTechRow = iTechGOMOrRow

		#Pick all techs in most advanced column
		aMostAdvancedColumnRequirementsNames = []
		for i in xrange(len(aTechGridXList)):
			if aTechGridXList[i] == max(aTechGridXList):
				aMostAdvancedColumnRequirementsNames.append(aTechList[i])

		#aMostAdvancedColumnRequirementsXY is a Tech location ID array - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech requirement, then both infotype X/Y grid is 0
		#Otherwise infotype gets highest Xgrid tech requirement and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		aMostAdvancedColumnRequirementsXY = []
		for i in xrange(len(aTechGridXList)):
			if aTechGridXList[i] == max(aTechGridXList):
				aMostAdvancedColumnRequirementsXY.append(aTechXY[i])

		if len(aMostAdvancedColumnRequirementsXY) == 0:
			aMostAdvancedColumnRequirementsXY.append(0)

		return iTechLoc, iTechRow, aMostAdvancedColumnRequirementsXY, aMostAdvancedColumnRequirementsNames

	#Unit tech location
	def checkUnitTechRequirementLocation(self, CvUnitInfo):
		#Main tech
		iTechMainReq = CvUnitInfo.getPrereqAndTech()
		if iTechMainReq != -1:
			iTechMainLoc = GC.getTechInfo(iTechMainReq).getGridX()
			iTechMainRow = GC.getTechInfo(iTechMainReq).getGridY()
		else:
			iTechMainLoc = 0
			iTechMainRow = 0

		#Tech Type requirement
		aTechTypeLocList = []
		aTechTypeRowList = []
		for iTechType in CvUnitInfo.getPrereqAndTechs():
			aTechTypeLocList.append(GC.getTechInfo(iTechType).getGridX())
			aTechTypeRowList.append(GC.getTechInfo(iTechType).getGridY())
		if len(aTechTypeLocList) > 0 and len(aTechTypeRowList) > 0:
			iTechTypeLoc = max(aTechTypeLocList)
			for iTechLoc in xrange(len(aTechTypeLocList)):
				if aTechTypeLocList[iTechLoc] == max(aTechTypeLocList):
					iTechTypeRow = aTechTypeRowList[iTechLoc]
		else:
			iTechTypeLoc = 0
			iTechTypeRow = 0

		#Tech GOM requirements
		aTechGOMReqList = []
		aTechGOMAndLocList = []
		aTechGOMAndRowList = []
		aTechGOMOrLocList = []
		aTechGOMOrRowList = []
		for i in range(2):
			aTechGOMReqList.append([])
		self.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_TECH, aTechGOMReqList)

		#Extract GOM AND requirements
		for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
			aTechGOMAndLocList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getGridX())
			aTechGOMAndRowList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getGridY())
		if len(aTechGOMAndLocList) > 0 and len(aTechGOMAndRowList) > 0:
			iTechGOMAndLoc = max(aTechGOMAndLocList)
			for iTechLoc in xrange(len(aTechGOMAndLocList)):
				if aTechGOMAndLocList[iTechLoc] == max(aTechGOMAndLocList):
					iTechGOMAndRow = aTechGOMAndRowList[iTechLoc]
		else:
			iTechGOMAndLoc = 0
			iTechGOMAndRow = 0

		#Extract GOM OR requirements
		for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
			aTechGOMOrLocList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR][iTech]).getGridX())
			aTechGOMOrRowList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR][iTech]).getGridY())
		if len(aTechGOMOrLocList) > 0 and len(aTechGOMOrRowList) > 0:
			iTechGOMOrLoc = min(aTechGOMOrLocList)
			for iTechLoc in xrange(len(aTechGOMOrLocList)):
				if aTechGOMOrLocList[iTechLoc] == min(aTechGOMOrLocList):
					iTechGOMOrRow = aTechGOMOrRowList[iTechLoc]
		else:
			iTechGOMOrLoc = 0
			iTechGOMOrRow = 0

		#Pick most advanced tech
		iTechLoc = max(iTechMainLoc, iTechTypeLoc, iTechGOMAndLoc, iTechGOMOrLoc)
		if iTechLoc == iTechMainLoc:
			iTechRow = iTechMainRow
		elif iTechLoc == iTechTypeLoc:
			iTechRow = iTechTypeRow
		elif iTechLoc == iTechGOMAndLoc:
			iTechRow = iTechGOMAndRow
		elif iTechLoc == iTechGOMOrLoc:
			iTechRow = iTechGOMOrRow

		#This is a Tech location ID - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech requirement, then both infotype X/Y grid is 0
		#Otherwise infotype gets highest Xgrid tech requirement and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		iTechXY = 100*iTechLoc + iTechRow
		sTechDesc = self.getTechName(iTechXY)

		return iTechLoc, iTechRow, iTechXY, sTechDesc

	#Promotion tech location
	def checkPromotionTechRequirementLocation(self, CvPromotionInfo):
		#Promotions have one tech requirement.
		TechReq = CvPromotionInfo.getTechPrereq()
		if TechReq != -1:
			iTechLoc = GC.getTechInfo(TechReq).getGridX()
			iTechRow = GC.getTechInfo(TechReq).getGridY()
		else:
			iTechLoc = 0
			iTechRow = 0

		#This is a Tech location ID - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech requirement, then both infotype X/Y grid is 0
		#Otherwise infotype gets highest Xgrid tech requirement and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		iTechXY = 100*iTechLoc + iTechRow
		sTechDesc = self.getTechName(iTechXY)

		return iTechLoc, iTechRow, iTechXY, sTechDesc

	#Bonus tech locations
	def checkBonusTechRequirementLocation(self, CvBonusInfo):
		#TechReveal - bonus shown on map, improvements and buildings can't provide it for empire yet, RawVicinity accesses resource like feature on map.
		#TechEnable - full access to bonus, can be traded
		TechReqReveal = CvBonusInfo.getTechReveal()
		TechReqEnable = CvBonusInfo.getTechCityTrade()

		if TechReqReveal != -1:
			iTechRevealLoc = GC.getTechInfo(TechReqReveal).getGridX()
			iTechRevealRow = GC.getTechInfo(TechReqReveal).getGridY()
		else:
			iTechRevealLoc = 0
			iTechRevealRow = 0

		if TechReqEnable != -1:
			iTechEnableLoc = GC.getTechInfo(TechReqEnable).getGridX()
			iTechEnableRow = GC.getTechInfo(TechReqEnable).getGridY()
		else:
			iTechEnableLoc = 0
			iTechEnableRow = 0


		#This is a Tech location ID - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech requirement, then both infotype X/Y grid is 0
		#Otherwise infotype gets highest Xgrid tech requirement and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		iTechRevealXY = 100*iTechRevealLoc + iTechRevealRow
		iTechEnableXY = 100*iTechEnableLoc + iTechEnableRow
		sTechDescReveal = self.getTechName(iTechRevealXY)
		sTechDescEnable = self.getTechName(iTechEnableXY)

		return iTechRevealLoc, iTechRevealRow, iTechEnableLoc, iTechEnableRow, iTechRevealXY, iTechEnableXY, sTechDescReveal, sTechDescEnable

	#Improvement tech location
	def checkImprovementTechRequirementLocation(self, CvImprovementInfo):
		#Improvements have one tech requirement
		TechReq = CvImprovementInfo.getPrereqTech()
		if TechReq != -1:
			iTechLoc = GC.getTechInfo(TechReq).getGridX()
			iTechRow = GC.getTechInfo(TechReq).getGridY()
		else:
			iTechLoc = 0
			iTechRow = 0

		#This is a Tech location ID - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech requirement, then both infotype X/Y grid is 0
		#Otherwise infotype gets highest Xgrid tech requirement and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		iTechXY = 100*iTechLoc + iTechRow
		sTechDesc = self.getTechName(iTechXY)

		return iTechLoc, iTechRow, iTechXY, sTechDesc

	#Civic tech location
	def checkCivicTechRequirementLocation(self, CvCivicInfo):
		#Civics have one tech requirement.
		TechReq = CvCivicInfo.getTechPrereq()
		if TechReq != -1:
			iTechLoc = GC.getTechInfo(TechReq).getGridX()
			iTechRow = GC.getTechInfo(TechReq).getGridY()
		else:
			iTechLoc = 0
			iTechRow = 0

		#This is a Tech location ID - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech requirement, then both infotype X/Y grid is 0
		#Otherwise infotype gets highest Xgrid tech requirement and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		iTechXY = 100*iTechLoc + iTechRow
		sTechDesc = self.getTechName(iTechXY)

		return iTechLoc, iTechRow, iTechXY, sTechDesc

	#Build tech location
	def checkBuildTechRequirementLocation(self, CvBuildInfo):
		#Builds have one tech requirement
		TechReq = CvBuildInfo.getTechPrereq()
		if TechReq != -1:
			iTechLoc = GC.getTechInfo(TechReq).getGridX()
			iTechRow = GC.getTechInfo(TechReq).getGridY()
		else:
			iTechLoc = 0
			iTechRow = 0

		#This is a Tech location ID - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech requirement, then both infotype X/Y grid is 0
		#Otherwise infotype gets highest Xgrid tech requirement and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		iTechXY = 100*iTechLoc + iTechRow
		sTechDesc = self.getTechName(iTechXY)

		return iTechLoc, iTechRow, iTechXY, sTechDesc

	#Tech name - obtained from tech ID
	def getTechName(self, iTechXY):
		global aTechID, aTechDesc
		#This loop runs once, as cache is created
		if len(aTechID) != GC.getNumTechInfos() or len(aTechDesc) != GC.getNumTechInfos():
			for iTech in xrange(GC.getNumTechInfos()):
				CvTechInfo = GC.getTechInfo(iTech)
				aTechID.append(100*CvTechInfo.getGridX() + CvTechInfo.getGridY())
				aTechDesc.append(CvTechInfo.getType())
				#Position of tech corresponds tech name

		if iTechXY != 0 and iTechXY != 99999: #If we have tech requirement or tech obsoletion
			return aTechDesc[aTechID.index(iTechXY)]
		else:
			return ""

	#^^^^ HIGHEST TECH REQUIREMENT LOCATION FINDER FUNCTIONS  ^^^^#

	##### GOM REQUIREMENT READER FUNCTIONS #####

	#Example use:
	#aGOMReqList = []
	#for i in range(2):
	#	aGOMReqList.append([])
	#getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BONUS, aGOMReqList) - for buildings
	#getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BONUS, aGOMReqList) - for units
	#Array is filled with enums - GC.getBonusInfo(aGOMReqList[BoolExprTypes.BOOLEXPR_AND][i]).getType() will extract type of bonus at i-th place.
	#Array is filled with enums - GC.getBonusInfo(aGOMReqList[BoolExprTypes.BOOLEXPR_OR][i]).getType() will extract type of bonus at i-th place.

	def getGOMReqs(self, CyBoolExpr, GOMType, GOMReqList, eParentExpr = BoolExprTypes.NO_BOOLEXPR):
		if CyBoolExpr is not None:
			eExpr = CyBoolExpr.getType()
			if eExpr == BoolExprTypes.BOOLEXPR_AND \
			or eExpr == BoolExprTypes.BOOLEXPR_OR:
				self.getGOMReqs(CyBoolExpr.getFirstExpr(), GOMType, GOMReqList, eExpr)
				self.getGOMReqs(CyBoolExpr.getSecondExpr(), GOMType, GOMReqList, eExpr)

			elif eExpr == BoolExprTypes.BOOLEXPR_HAS and CyBoolExpr.getGOMType() == GOMType:
				GOMReqList[eParentExpr].append(CyBoolExpr.getID())

	#^^^^ GOM REQUIREMENT READER FUNCTIONS ^^^^#

	##### OBSOLETION TECH LOCATION FINDER FUNCTIONS #####

	#Building tech obsoletion location
	def checkBuildingTechObsoletionLocation(self, CvBuildingInfo):
		iTechObsLoc = 999 #Never obsoletes
		if CvBuildingInfo.getObsoleteTech() != -1:
			iTechObsLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()

		iTechObsRow = 99 #Never obsoletes
		if CvBuildingInfo.getObsoleteTech() != -1:
			iTechObsRow = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridY()

		#This is a Tech location ID - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech obsoletion, then infotype X/Y grid is 999 / 99
		#Otherwise infotype gets highest Xgrid tech obsoletion and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		iTechObsXY = 100*iTechObsLoc + iTechObsRow
		sTechDesc = self.getTechName(iTechObsXY)

		return iTechObsLoc, iTechObsXY, sTechDesc

	#^^^^ OBSOLETION TECH LOCATION FINDER FUNCTIONS ^^^^#

	#Building requirements of buildings
	def checkBuildingRequirements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iBaseTechLoc = self.checkBuildingTechRequirements(CvBuildingInfo)[0]
			aBaseTechIDList = self.checkBuildingTechRequirements(CvBuildingInfo)[2]
			aBaseTechList = self.checkBuildingTechRequirements(CvBuildingInfo)[3]

			#<PrereqInCityBuildings> - require all buildings in list
			aRequirementTechLocList = []
			aReqColumnTechIDList = []
			aReqColumnTechList = []
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
				aRequirementTechLocList.append(self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
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
				aRequirementTechLocList.append(self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
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
			for pair in CvBuildingInfo.getPrereqNumOfBuildings():
				iPrereqBuilding = pair.id
				aRequirementTechLocList.append(self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
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
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aRequirementTechLocList = []
			aReqColumnTechIDList = []
			aReqColumnTechList = []
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuildingRequirement]
				aRequirementTechLocList.append(self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
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
				aRequirementTechLocList.append(self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[0])
				aRequirementTechIDList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[2]
				aRequirementTechList = self.checkBuildingTechRequirements(GC.getBuildingInfo(iPrereqBuilding))[3]

				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechIDList.append(aRequirementTechIDList[i])
				for i in xrange(len(aRequirementTechList)):
					if GC.getTechInfo(GC.getInfoTypeForString(aRequirementTechList[i])).getGridX() == iBaseTechLoc:
						aReqColumnTechList.append(aRequirementTechList[i])
			if len(aRequirementTechLocList) > 0 and min(aRequirementTechLocList) > iBaseTechLoc:
				self.log(CvBuildingInfo.getType()+" is unlocked before its GOM OR building requirements "+str(aRequirementTechList)+" "+str(aBaseTechList)+" requirement/base column "+str(min(aRequirementTechLocList))+" / "+str(iBaseTechLoc))
			if len(aRequirementTechLocList) > 0 and min(aRequirementTechLocList) == iBaseTechLoc:
				for i in xrange(len(aReqColumnTechIDList)):
					if aReqColumnTechIDList[i] not in aBaseTechIDList:
						self.log(CvBuildingInfo.getType()+" GOM OR requirement unlocked by different tech in column: "+str(iBaseTechLoc)+" requirement/base "+str(aReqColumnTechList[i])+" / "+str(aBaseTechList))

	#Building replacements of requirements
	def checkBuildingRequirementReplacements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iFreeBuilding = CvBuildingInfo.getFreeBuilding()
			aBuildingRequirementList = []
			aBuildingRequirementNamesList = []

			#<PrereqInCityBuildings> - require all buildings in list
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
				aBuildingRequirementList.append(iPrereqBuilding)
				aBuildingRequirementNamesList.append(GC.getBuildingInfo(iPrereqBuilding).getType())

			#<PrereqOrBuildings> - require one building in list
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuildingRequirement)
				if iPrereqBuilding not in aBuildingRequirementList:
					aBuildingRequirementList.append(iPrereqBuilding)
					aBuildingRequirementNamesList.append(GC.getBuildingInfo(iPrereqBuilding).getType())

			#<PrereqAmountBuildings> - require all buildings in empire in list
			for pair in CvBuildingInfo.getPrereqNumOfBuildings():
				iPrereqBuilding = pair.id
				if iPrereqBuilding not in aBuildingRequirementList:
					aBuildingRequirementList.append(iPrereqBuilding)
					aBuildingRequirementNamesList.append(GC.getBuildingInfo(iPrereqBuilding).getType())

			#<ConstructCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuildingRequirement]
				if iPrereqBuilding not in aBuildingRequirementList:
					aBuildingRequirementList.append(iPrereqBuilding)
					aBuildingRequirementNamesList.append(GC.getBuildingInfo(iPrereqBuilding).getType())

			#Analyze GOM OR Building reqs
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuildingRequirement]
				if iPrereqBuilding not in aBuildingRequirementList:
					aBuildingRequirementList.append(iPrereqBuilding)
					aBuildingRequirementNamesList.append(GC.getBuildingInfo(iPrereqBuilding).getType())

			#Generate list of buildings, that replace requirements
			aBuildingRequirementReplacementList = []
			for i in xrange(len(aBuildingRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingRequirementList[i])
				for iBuildingReplacement in xrange(CvBuildingRequirementInfo.getNumReplacementBuilding()):
					iReplacementBuilding = CvBuildingRequirementInfo.getReplacementBuilding(iBuildingReplacement)
					if iReplacementBuilding not in aBuildingRequirementReplacementList:
						aBuildingRequirementReplacementList.append(iReplacementBuilding)

			#Remove buildings from requirement replacement list, if they already exist in requirement list
			aBuildingRequirementReplacementUniqueList = []
			aBuildingRequirementReplacementUniqueNameList = []
			for i in xrange(len(aBuildingRequirementReplacementList)):
				if aBuildingRequirementReplacementList[i] not in aBuildingRequirementList:
					aBuildingRequirementReplacementUniqueList.append(aBuildingRequirementReplacementList[i])
					aBuildingRequirementReplacementUniqueNameList.append(GC.getBuildingInfo(aBuildingRequirementReplacementList[i]).getType())

			#Get replacements of base building
			aBuildingBaseReplacementList = []
			for iBuildingReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				iReplacementBuilding = CvBuildingInfo.getReplacementBuilding(iBuildingReplacement)
				aBuildingBaseReplacementList.append(GC.getBuildingInfo(iReplacementBuilding).getType())

			#If we have unlisted replacements
			#They don't have to be listed if:
			#Requirement replaces base building
			#Requirement and base building is replaced
			#Free building from base building is replacement of requirement

			if len(aBuildingRequirementReplacementUniqueList) > 0 and iBuilding not in aBuildingRequirementReplacementUniqueList and aBuildingRequirementReplacementUniqueList > aBuildingBaseReplacementList and iFreeBuilding not in aBuildingRequirementReplacementUniqueList:
				self.log(CvBuildingInfo.getType()+" requirements "+str(aBuildingRequirementNamesList)+" have unlisted replacements "+str(aBuildingRequirementReplacementUniqueNameList))

	#Building obsoletion of requirements - requirements shouldn't obsolete before building itself
	def checkBuildingRequirementObsoletion(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			BuildingObsoleteTechLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
			BuildingObsoleteTechID = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[1]

			#<PrereqInCityBuildings> - require all buildings in list
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementObsoleteTechIDList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuilding)
				aBuildingRequirementObsoleteTechLocList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
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
				aBuildingRequirementObsoleteTechLocList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
				aBuildingRequirementNameList.append(GC.getBuildingInfo(iPrereqBuilding).getType())
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has latest OR requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList)-10 <= BuildingObsoleteTechLoc and max(aBuildingRequirementObsoleteTechLocList) < 999 and BuildingObsoleteTechID not in aBuildingRequirementObsoleteTechIDList:
				self.log(CvBuildingInfo.getType()+" has latest OR requirements obsolete fairly soon after base building - consider picking its obsoletion tech "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechIDList)+" "+str(BuildingObsoleteTechID))

			#<PrereqAmountBuildings> - require all buildings in empire in list
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementObsoleteTechIDList = []
			aBuildingRequirementNameList = []
			for pair in CvBuildingInfo.getPrereqNumOfBuildings():
				iPrereqBuilding = pair.id
				aBuildingRequirementObsoleteTechLocList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
				aBuildingRequirementNameList.append(GC.getBuildingInfo(iPrereqBuilding).getType())
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has Empire AND requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList)-10 <= BuildingObsoleteTechLoc and min(aBuildingRequirementObsoleteTechLocList) < 999 and BuildingObsoleteTechID not in aBuildingRequirementObsoleteTechIDList:
				self.log(CvBuildingInfo.getType()+" has Empire AND requirements obsolete fairly soon after base building - consider picking its obsoletion tech "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechIDList)+" "+str(BuildingObsoleteTechID))

			#<ConstructCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementObsoleteTechIDList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]
				aBuildingRequirementObsoleteTechLocList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
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
				aBuildingRequirementObsoleteTechLocList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				aBuildingRequirementObsoleteTechIDList.append(self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[1])
				aBuildingRequirementNameList.append(GC.getBuildingInfo(iPrereqBuilding).getType())
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has latest GOM OR requirement obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList)-10 <= BuildingObsoleteTechLoc and max(aBuildingRequirementObsoleteTechLocList) < 999 and BuildingObsoleteTechID not in aBuildingRequirementObsoleteTechIDList:
				self.log(CvBuildingInfo.getType()+" has latest GOM OR requirement obsolete fairly soon after base building - consider picking its obsoletion tech "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechIDList)+" "+str(BuildingObsoleteTechID))

	#Buildings shouldn't obsolete too fast in relation of tech unlock
	def checkBuildingUnlockObsoletion(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirements(CvBuildingInfo)[0]
			if CvBuildingInfo.getObsoleteTech() != -1:
				iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
				if iObsoleteTechLoc - iTechLoc <= 10:
					self.log(CvBuildingInfo.getType()+" Unlock: "+str(iTechLoc)+" Obsoletion: "+str(iObsoleteTechLoc)+" Difference: "+str(iObsoleteTechLoc - iTechLoc))

	#Building replacements shouldn't obsolete too fast for sanity of beeliners, replacements also shouldn't obsolete at earlier point compared to base
	def checkBuildingReplacementObsoletion(self):
		aSpecialReplacementsList = [GC.getInfoTypeForString("BUILDING_POLLUTION_BLACKENEDSKIES"), GC.getInfoTypeForString("BUILDING_GAMBLING_BAN"), GC.getInfoTypeForString("BUILDING_ALCOCHOL_PROHIBITION"), GC.getInfoTypeForString("BUILDING_DRUG_PROHIBITION"), GC.getInfoTypeForString("BUILDING_PROSTITUTION_BAN")]
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirements(CvBuildingInfo)[0]
			iTechID = max(self.checkBuildingTechRequirements(CvBuildingInfo)[2])
			iTechObsLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
			iTechObsID = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[1]

			#All replacements of base
			aBuildingReplacementList = []
			for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				iBuildingReplacement = CvBuildingInfo.getReplacementBuilding(iReplacement)
				aBuildingReplacementList.append(iBuildingReplacement)

			#All replacements of replacements
			aBuildingReplacement2List = []
			aBuildingReplacement2NameList = []
			for i in xrange(len(aBuildingReplacementList)):
				CvBuildingReplacementInfo = GC.getBuildingInfo(aBuildingReplacementList[i])
				for iReplacement2 in xrange(CvBuildingReplacementInfo.getNumReplacementBuilding()):
					iBuildingReplacement2 = CvBuildingReplacementInfo.getReplacementBuilding(iReplacement2)
					if iBuildingReplacement2 not in aBuildingReplacement2List:
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
					iImmediateReplacementTechLoc = self.checkBuildingTechRequirements(CvBuildingImmediateReplacementInfo)[0]
					iImmediateReplacementTechLocID = max(self.checkBuildingTechRequirements(CvBuildingImmediateReplacementInfo)[2])
					iImmediateReplacementTechObs = self.checkBuildingTechObsoletionLocation(CvBuildingImmediateReplacementInfo)[0]
					iImmediateReplacementTechObsID = self.checkBuildingTechObsoletionLocation(CvBuildingImmediateReplacementInfo)[1]
					if aImmediateReplacementList[i] not in aSpecialReplacementsList and iBuilding != GC.getInfoTypeForString("BUILDING_HOMELESS"):
						if iTechObsLoc < iImmediateReplacementTechLoc:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBuildingImmediateReplacementInfo.getType()+" Base obsoletion/Replacement unlock "+str(iTechObsLoc)+"/"+str(iImmediateReplacementTechLoc))
						if (iTechObsLoc > iImmediateReplacementTechObs or (iTechObsLoc == iImmediateReplacementTechObs and iTechObsID != iImmediateReplacementTechObsID)) and CvBuildingImmediateReplacementInfo.getType().find("_STORIES_EFFECT", -15) == -1:
							self.log(CvBuildingInfo.getType()+" obsoletes after or concurrently with "+CvBuildingImmediateReplacementInfo.getType()+" Base/Replacement obsoletions "+str(iTechObsLoc)+"/"+str(iImmediateReplacementTechObs))
						if (iTechLoc > iImmediateReplacementTechLoc or (iTechLoc == iImmediateReplacementTechLoc and iTechID != iImmediateReplacementTechLocID)) and CvBuildingImmediateReplacementInfo.getType().find("_STORIES_EFFECT", -15) == -1:
							self.log(CvBuildingInfo.getType()+" unlocks after or concurrently with "+CvBuildingImmediateReplacementInfo.getType()+" Base/Replacement unlocks "+str(iTechLoc)+"/"+str(iImmediateReplacementTechLoc))
						if iImmediateReplacementTechObs == 999 and len(aBuildingReplacement2List) > 0 and CvBuildingImmediateReplacementInfo.getType().find("_STORIES_EFFECT", -15) == -1:
							self.log(CvBuildingInfo.getType()+" replacement doesn't obsolete, but replacements of replacement exist "+CvBuildingImmediateReplacementInfo.getType()+" Base/Replacement obsoletions "+str(iTechObsLoc)+"/"+str(iImmediateReplacementTechObs)+" replacements of replacement "+str(aBuildingReplacement2NameList))

			#Get replacements of replacements
			for i in xrange(len(aImmediateReplacementList)):
				Cv2BuildingInfo = GC.getBuildingInfo(aImmediateReplacementList[i])
				iReplacementTechLoc = self.checkBuildingTechRequirements(Cv2BuildingInfo)[0]

				#All replacements of replacements
				a2BuildingReplacementList = []
				for iReplacement in xrange(Cv2BuildingInfo.getNumReplacementBuilding()):
					iBuildingReplacement = Cv2BuildingInfo.getReplacementBuilding(iReplacement)
					a2BuildingReplacementList.append(iBuildingReplacement)

				#All third level replacements
				a2BuildingReplacement2List = []
				for i in xrange(len(a2BuildingReplacementList)):
					Cv2BuildingReplacementInfo = GC.getBuildingInfo(a2BuildingReplacementList[i])
					for iReplacement2 in xrange(Cv2BuildingReplacementInfo.getNumReplacementBuilding()):
						iBuildingReplacement2 = Cv2BuildingReplacementInfo.getReplacementBuilding(iReplacement2)
						if iBuildingReplacement2 not in a2BuildingReplacement2List:
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
						iReplacement2TechLoc = self.checkBuildingTechRequirements(Cv2BuildingImmediateReplacementInfo)[0]
						if iTechObsLoc > iReplacement2TechLoc and aImmediateReplacement2List[i] not in aSpecialReplacementsList:
							if iTechObsLoc - iReplacementTechLoc <= 10 and iBuilding != GC.getInfoTypeForString("BUILDING_HOMELESS"):
								self.log(CvBuildingInfo.getType()+" -> "+Cv2BuildingImmediateReplacementInfo.getType()+" Base obsoletion/Second lvl replacement unlock - consider picking more advanced tech "+str(iTechObsLoc)+"/"+str(iReplacement2TechLoc))

	#Building - Check if we don't have implicit replacements, also ensure that listed ones aren't unlocked before building
	def checkBuildingImplicitReplacements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iBaseTechLoc = self.checkBuildingTechRequirements(CvBuildingInfo)[0]

			#Replacements
			aReplacementBuildingList = []
			aReplacementBuildingTechLocList = []
			for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				iReplacementBuilding = CvBuildingInfo.getReplacementBuilding(i)
				CvBuildingReplacementInfo = GC.getBuildingInfo(iReplacementBuilding)
				aReplacementBuildingList.append(iReplacementBuilding)
				iReplacementTechLoc = self.checkBuildingTechRequirements(CvBuildingReplacementInfo)[0]
				if iBaseTechLoc >= iReplacementTechLoc and iBaseTechLoc > 0 and iReplacementTechLoc > 0:
					self.log("WARNING: "+CvBuildingInfo.getType()+" is unlocked after or concurrently with "+CvBuildingReplacementInfo.getType()+" "+str(iBaseTechLoc)+"/"+str(iReplacementTechLoc))

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
	def checkBuildingReplacingProductivity(self):
		aSpecialBuildingsList = [GC.getInfoTypeForString("BUILDING_POLLUTION_BLACKENEDSKIES"), GC.getInfoTypeForString("BUILDING_GAMBLING_BAN"), GC.getInfoTypeForString("BUILDING_ALCOCHOL_PROHIBITION"), GC.getInfoTypeForString("BUILDING_DRUG_PROHIBITION"), GC.getInfoTypeForString("BUILDING_PROSTITUTION_BAN")]
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			#Tech location would be good way to sort replacements, as later ones tend to replace more
			iTechID = max(self.checkBuildingTechRequirements(CvBuildingInfo)[2])

			#Ignore Stories Effect, Pollution, and Bans
			if iBuilding not in aSpecialBuildingsList and CvBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1 and CvBuildingInfo.getNumReplacedBuilding() != 0:
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
				aImmediateReplacedNameList = []
				for i in xrange(len(aReplacedBuildings)):
					if aReplacedBuildings[i] not in aBuildingReplaced2List:
						CvBuildingReplacedInfo = GC.getBuildingInfo(aReplacedBuildings[i])
						aImmediateReplacedList.append(aReplacedBuildings[i])
						aImmediateReplacedNameList.append(GC.getBuildingInfo(aReplacedBuildings[i]).getType())

				#===== 0D ENTRIES - INTEGERS ===========================================================================================================================#
				#<iTradeRoutes>, <iCoastalTradeRoutes>, <iGlobalTradeRoutes>, <iTradeRouteModifier>, <iForeignTradeRouteModifier>, <iHappiness>, <iHealth>, <iGreatPeopleRateChange>, <iGreatPeopleRateModifier>, <iFreeSpecialist>, <iAreaFreeSpecialist>, <iGlobalFreeSpecialist>, <iMaintenanceModifier>, <iHappinessPercentPerPopulation>, <iHealthPercentPerPopulation>, <iWarWearinessModifier>, <iGlobalWarWearinessModifier>, <iEnemyWarWearinessModifier>, <iAllCityDefense>, <iBombardDefense>, <iBuildingDefenseRecoverySpeedModifier>, <iCityDefenseRecoverySpeedModifier>, <iDefense>, <iEspionageDefense>, <iLocalDynamicDefense>, <iMinDefense>, <iNoEntryDefenseLevel>, <iRiverDefensePenalty>, <iExperience>, <iGlobalExperience> - base
				iBaseTradeRoutes = CvBuildingInfo.getTradeRoutes()
				iBaseCoastalTradeRoutes = CvBuildingInfo.getCoastalTradeRoutes()
				iBaseGlobalTradeRoutes = CvBuildingInfo.getGlobalTradeRoutes()
				iBaseTradeRouteModifier = CvBuildingInfo.getTradeRouteModifier()
				iBaseForeignTradeRouteModifier = CvBuildingInfo.getForeignTradeRouteModifier()
				iBaseHappiness = CvBuildingInfo.getHappiness()
				iBaseHealth = CvBuildingInfo.getHealth()
				iBaseGreatPeopleRateChange = CvBuildingInfo.getGreatPeopleRateChange()
				iBaseGreatPeopleRateModifier = CvBuildingInfo.getGreatPeopleRateModifier()
				iBaseFreeSpecialist = CvBuildingInfo.getFreeSpecialist()
				iBaseAreaFreeSpecialist = CvBuildingInfo.getAreaFreeSpecialist()
				iBaseGlobalFreeSpecialist = CvBuildingInfo.getGlobalFreeSpecialist()
				iBaseMaintenanceModifier = CvBuildingInfo.getMaintenanceModifier() #iMaintenanceModifier - lower value is better
				iBaseHappinessPercentPerPopulation = CvBuildingInfo.getHappinessPercentPerPopulation()
				iBaseHealthPercentPerPopulation = CvBuildingInfo.getHealthPercentPerPopulation()
				iBaseWarWearinessModifier = CvBuildingInfo.getWarWearinessModifier() #iWarWearinessModifier - lower value is better
				iBaseGlobalWarWearinessModifier = CvBuildingInfo.getGlobalWarWearinessModifier() #iGlobalWarWearinessModifier - lower value is better
				iBaseEnemyWarWearinessModifier = CvBuildingInfo.getEnemyWarWearinessModifier()
				iBaseAllCityDefense = CvBuildingInfo.getAllCityDefenseModifier()
				iBaseBombardDefense = CvBuildingInfo.getBombardDefenseModifier()
				iBaseBuildingDefenseRecoverySpeedModifier = CvBuildingInfo.getBuildingDefenseRecoverySpeedModifier()
				iBaseCityDefenseRecoverySpeedModifier = CvBuildingInfo.getCityDefenseRecoverySpeedModifier()
				iBaseDefense = CvBuildingInfo.getDefenseModifier()
				iBaseEspionageDefense = CvBuildingInfo.getEspionageDefenseModifier()
				iBaseLocalDynamicDefense = CvBuildingInfo.getLocalDynamicDefense()
				iBaseMinDefense = CvBuildingInfo.getMinDefense()
				iBaseNoEntryDefenseLevel = CvBuildingInfo.getNoEntryDefenseLevel()  #iNoEntryDefenseLevel - lower value is better
				iBaseRiverDefensePenalty = CvBuildingInfo.getRiverDefensePenalty()
				iBaseExperience = CvBuildingInfo.getFreeExperience()
				iBaseGlobalExperience = CvBuildingInfo.getGlobalFreeExperience()

				#Analyze replacements by tag
				iTradeRoutes = 0
				iCoastalTradeRoutes = 0
				iGlobalTradeRoutes = 0
				iTradeRouteModifier = 0
				iForeignTradeRouteModifier = 0
				iHappiness = 0
				iHealth = 0
				iGreatPeopleRateChange = 0
				iGreatPeopleRateModifier = 0
				iFreeSpecialist = 0
				iAreaFreeSpecialist = 0
				iGlobalFreeSpecialist = 0
				iMaintenanceModifier = 0 #iMaintenanceModifier - lower value is better
				iHappinessPercentPerPopulation = 0
				iHealthPercentPerPopulation = 0
				iWarWearinessModifier = 0 #iWarWearinessModifier - lower value is better
				iGlobalWarWearinessModifier = 0 #iGlobalWarWearinessModifier - lower value is better
				iEnemyWarWearinessModifier = 0
				iAllCityDefense = 0
				iBombardDefense = 0
				iBuildingDefenseRecoverySpeedModifier = 0
				iCityDefenseRecoverySpeedModifier = 0
				iDefense = 0
				iEspionageDefense = 0
				iLocalDynamicDefense = 0
				iMinDefense = 0
				iNoEntryDefenseLevel = 0  #iNoEntryDefenseLevel - lower value is better
				iRiverDefensePenalty = 0
				iExperience = 0
				iGlobalExperience = 0
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<iTradeRoutes>, <iCoastalTradeRoutes>, <iGlobalTradeRoutes>, <iTradeRouteModifier>, <iForeignTradeRouteModifier>, <iHappiness>, <iHealth>, <iGreatPeopleRateChange>, <iGreatPeopleRateModifier>, <iFreeSpecialist>, <iAreaFreeSpecialist>, <iGlobalFreeSpecialist>, <iMaintenanceModifier>, <iHappinessPercentPerPopulation>, <iHealthPercentPerPopulation>, <iWarWearinessModifier>, <iGlobalWarWearinessModifier>, <iEnemyWarWearinessModifier>, <iAllCityDefense>, <iBombardDefense>, <iBuildingDefenseRecoverySpeedModifier>, <iCityDefenseRecoverySpeedModifier>, <iDefense>, <iEspionageDefense>, <iLocalDynamicDefense>, <iMinDefense>, <iNoEntryDefenseLevel>, <iRiverDefensePenalty>, <iExperience>, <iGlobalExperience>
					iTradeRoutes += CvReplacedBuildingInfo.getTradeRoutes()
					iCoastalTradeRoutes += CvReplacedBuildingInfo.getCoastalTradeRoutes()
					iGlobalTradeRoutes += CvReplacedBuildingInfo.getGlobalTradeRoutes()
					iTradeRouteModifier += CvReplacedBuildingInfo.getTradeRouteModifier()
					iForeignTradeRouteModifier += CvReplacedBuildingInfo.getForeignTradeRouteModifier()
					iHappiness += CvReplacedBuildingInfo.getHappiness()
					iHealth += CvReplacedBuildingInfo.getHealth()
					iGreatPeopleRateChange += CvReplacedBuildingInfo.getGreatPeopleRateChange()
					if CvBuildingInfo.getGreatPeopleUnitType() != -1 and CvReplacedBuildingInfo.getGreatPeopleUnitType() != -1 and CvBuildingInfo.getGreatPeopleUnitType() != CvReplacedBuildingInfo.getGreatPeopleUnitType():
						self.log("WARNING: "+CvBuildingInfo.getType()+", "+CvReplacedBuildingInfo.getType()+" have different GPs, remove GPUnitType from both buildings, or unify entire replacement tree")
					iGreatPeopleRateModifier += CvReplacedBuildingInfo.getGreatPeopleRateModifier()
					iFreeSpecialist += CvReplacedBuildingInfo.getFreeSpecialist()
					iAreaFreeSpecialist += CvReplacedBuildingInfo.getAreaFreeSpecialist()
					iGlobalFreeSpecialist += CvReplacedBuildingInfo.getGlobalFreeSpecialist()
					iMaintenanceModifier += CvReplacedBuildingInfo.getMaintenanceModifier() #iMaintenanceModifier - lower value is better
					iHappinessPercentPerPopulation += CvReplacedBuildingInfo.getHappinessPercentPerPopulation()
					iHealthPercentPerPopulation += CvReplacedBuildingInfo.getHealthPercentPerPopulation()
					iWarWearinessModifier += CvReplacedBuildingInfo.getWarWearinessModifier() #iWarWearinessModifier - lower value is better
					iGlobalWarWearinessModifier += CvReplacedBuildingInfo.getGlobalWarWearinessModifier() #iGlobalWarWearinessModifier - lower value is better
					iEnemyWarWearinessModifier += CvReplacedBuildingInfo.getEnemyWarWearinessModifier()
					iAllCityDefense = CvReplacedBuildingInfo.getAllCityDefenseModifier()
					iBombardDefense = CvReplacedBuildingInfo.getBombardDefenseModifier()
					iBuildingDefenseRecoverySpeedModifier = CvReplacedBuildingInfo.getBuildingDefenseRecoverySpeedModifier()
					iCityDefenseRecoverySpeedModifier = CvReplacedBuildingInfo.getCityDefenseRecoverySpeedModifier()
					iDefense = CvReplacedBuildingInfo.getDefenseModifier()
					iEspionageDefense = CvReplacedBuildingInfo.getEspionageDefenseModifier()
					iLocalDynamicDefense = CvReplacedBuildingInfo.getLocalDynamicDefense()
					iMinDefense = CvReplacedBuildingInfo.getMinDefense()
					iNoEntryDefenseLevel = CvReplacedBuildingInfo.getNoEntryDefenseLevel()  #iNoEntryDefenseLevel - lower value is better
					iRiverDefensePenalty = CvReplacedBuildingInfo.getRiverDefensePenalty()
					iExperience = CvReplacedBuildingInfo.getFreeExperience()
					iGlobalExperience = CvReplacedBuildingInfo.getGlobalFreeExperience()

				#Keep already existing <iTradeRoutes>, <iCoastalTradeRoutes>, <iGlobalTradeRoutes>, <iTradeRouteModifier>, <iForeignTradeRouteModifier>, <iHappiness>, <iHealth>, <iGreatPeopleRateChange>, <iGreatPeopleRateModifier>, <iFreeSpecialist>, <iAreaFreeSpecialist>, <iGlobalFreeSpecialist>, <iMaintenanceModifier>, <iHappinessPercentPerPopulation>, <iHealthPercentPerPopulation>, <iWarWearinessModifier>, <iGlobalWarWearinessModifier>, <iEnemyWarWearinessModifier>, <iAllCityDefense>, <iBombardDefense>, <iBuildingDefenseRecoverySpeedModifier>, <iCityDefenseRecoverySpeedModifier>, <iDefense>, <iEspionageDefense>, <iLocalDynamicDefense>, <iMinDefense>, <iNoEntryDefenseLevel>, <iRiverDefensePenalty>, <iExperience>, <iGlobalExperience> in base
				iFinalTradeRoutes = iBaseTradeRoutes + iTradeRoutes
				iFinalCoastalTradeRoutes = iBaseCoastalTradeRoutes + iCoastalTradeRoutes
				iFinalGlobalTradeRoutes = iBaseGlobalTradeRoutes + iGlobalTradeRoutes
				iFinalTradeRouteModifier = iBaseTradeRouteModifier + iTradeRouteModifier
				iFinalForeignTradeRouteModifier = iBaseForeignTradeRouteModifier + iForeignTradeRouteModifier
				iFinalHappiness = iBaseHappiness + iHappiness
				iFinalHealth = iBaseHealth + iHealth
				iFinalGreatPeopleRateChange = iBaseGreatPeopleRateChange + iGreatPeopleRateChange
				iFinalGreatPeopleRateModifier = iBaseGreatPeopleRateModifier + iGreatPeopleRateModifier
				iFinalFreeSpecialist = iBaseFreeSpecialist + iFreeSpecialist
				iFinalAreaFreeSpecialist = iBaseAreaFreeSpecialist + iAreaFreeSpecialist
				iFinalGlobalFreeSpecialist = iBaseGlobalFreeSpecialist + iGlobalFreeSpecialist
				iFinalMaintenanceModifier = iBaseMaintenanceModifier + iMaintenanceModifier #iMaintenanceModifier - lower value is better
				iFinalHappinessPercentPerPopulation = iBaseHappinessPercentPerPopulation + iHappinessPercentPerPopulation
				iFinalHealthPercentPerPopulation = iBaseHealthPercentPerPopulation + iHealthPercentPerPopulation
				iFinalWarWearinessModifier = iBaseWarWearinessModifier + iWarWearinessModifier #iWarWearinessModifier - lower value is better
				iFinalGlobalWarWearinessModifier = iBaseGlobalWarWearinessModifier + iGlobalWarWearinessModifier #iGlobalWarWearinessModifier - lower value is better
				iFinalEnemyWarWearinessModifier = iBaseEnemyWarWearinessModifier + iEnemyWarWearinessModifier
				iFinalAllCityDefense = iBaseAllCityDefense + iAllCityDefense
				iFinalBombardDefense = iBaseBombardDefense + iBombardDefense
				iFinalBuildingDefenseRecoverySpeedModifier = iBaseBuildingDefenseRecoverySpeedModifier + iBuildingDefenseRecoverySpeedModifier
				iFinalCityDefenseRecoverySpeedModifier = iBaseCityDefenseRecoverySpeedModifier + iCityDefenseRecoverySpeedModifier
				iFinalDefense = iBaseDefense + iDefense
				iFinalEspionageDefense = iBaseEspionageDefense + iEspionageDefense
				iFinalLocalDynamicDefense = iBaseLocalDynamicDefense + iLocalDynamicDefense
				iFinalMinDefense = iBaseMinDefense + iMinDefense
				iFinalNoEntryDefenseLevel = iBaseNoEntryDefenseLevel + iNoEntryDefenseLevel  #iNoEntryDefenseLevel - lower value is better
				iFinalRiverDefensePenalty = iBaseRiverDefensePenalty + iRiverDefensePenalty
				iFinalExperience = iBaseExperience + iExperience
				iFinalGlobalExperience = iBaseGlobalExperience + iGlobalExperience

				#Building shouldn't be worse than replaced one!
				if iBaseTradeRoutes < iTradeRoutes:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Trade Routes "+str(iFinalTradeRoutes)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseCoastalTradeRoutes < iCoastalTradeRoutes:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Coastal Trade Routes "+str(iFinalCoastalTradeRoutes)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseGlobalTradeRoutes < iGlobalTradeRoutes:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global Trade Routes "+str(iFinalGlobalTradeRoutes)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseTradeRouteModifier < iTradeRouteModifier:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Trade Route Modifier "+str(iFinalTradeRouteModifier)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseForeignTradeRouteModifier < iForeignTradeRouteModifier:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Foreign Trade Route Modifier "+str(iFinalForeignTradeRouteModifier)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseHappiness < iHappiness:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Happiness "+str(iFinalHappiness)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseHealth < iHealth:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Health "+str(iFinalHealth)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseGreatPeopleRateChange < iGreatPeopleRateChange:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have GP Rate Change "+str(iFinalGreatPeopleRateChange)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseGreatPeopleRateModifier < iGreatPeopleRateModifier:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have GP Rate Modifier "+str(iFinalGreatPeopleRateModifier)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseFreeSpecialist < iFreeSpecialist:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Free Specialist "+str(iFinalFreeSpecialist)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseAreaFreeSpecialist < iAreaFreeSpecialist:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Area free Specialist "+str(iFinalAreaFreeSpecialist)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseGlobalFreeSpecialist < iGlobalFreeSpecialist:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global free Specialist "+str(iFinalGlobalFreeSpecialist)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseMaintenanceModifier > iMaintenanceModifier: #iMaintenanceModifier - lower value is better, as it increases cost of running city
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Maintaince Modifier "+str(iFinalMaintenanceModifier)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseHappinessPercentPerPopulation < iHappinessPercentPerPopulation:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Happiness per Pop "+str(iFinalHappinessPercentPerPopulation)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseHealthPercentPerPopulation < iHealthPercentPerPopulation:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Health per Pop "+str(iFinalHealthPercentPerPopulation)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseWarWearinessModifier > iWarWearinessModifier: #iWarWearinessModifier - lower value is better, as it increases unhappiness from war
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have War Weariness "+str(iFinalWarWearinessModifier)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseGlobalWarWearinessModifier > iGlobalWarWearinessModifier: #iGlobalWarWearinessModifier - lower value is better, as it increases unhappiness from war
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Global War Weariness "+str(iFinalGlobalWarWearinessModifier)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseEnemyWarWearinessModifier < iEnemyWarWearinessModifier:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Enemy War Weariness "+str(iFinalEnemyWarWearinessModifier)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseAllCityDefense < iAllCityDefense:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have All City Defense "+str(iFinalAllCityDefense)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseBombardDefense < iBombardDefense:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Bombard Defense "+str(iFinalBombardDefense)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseBuildingDefenseRecoverySpeedModifier < iBuildingDefenseRecoverySpeedModifier:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Building Defense Recovery Speed "+str(iFinalBuildingDefenseRecoverySpeedModifier)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseCityDefenseRecoverySpeedModifier < iCityDefenseRecoverySpeedModifier:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have City Defense Recovery Speed "+str(iFinalCityDefenseRecoverySpeedModifier)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseDefense < iDefense:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Defense "+str(iFinalDefense)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseEspionageDefense < iEspionageDefense:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Espionage Defense "+str(iFinalEspionageDefense)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseLocalDynamicDefense < iLocalDynamicDefense:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Local Dynamic Defense "+str(iFinalLocalDynamicDefense)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseMinDefense < iMinDefense:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Min Defense "+str(iFinalMinDefense)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseNoEntryDefenseLevel > iNoEntryDefenseLevel and not (iBaseNoEntryDefenseLevel > 0 and iNoEntryDefenseLevel == 0):  #iNoEntryDefenseLevel - lower value is better, as units have to dig trough to lower level of defense, value of 0 means no need to dig trough defense.
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have No Entry Defense Level "+str(iFinalNoEntryDefenseLevel)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseRiverDefensePenalty < iRiverDefensePenalty:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have River Defense Penalty "+str(iFinalRiverDefensePenalty)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseExperience < iExperience:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Free Experience "+str(iFinalExperience)+" replaced: "+str(aImmediateReplacedNameList))
				if iBaseGlobalExperience < iGlobalExperience:
					self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have Free global Experience Penalty "+str(iFinalGlobalExperience)+" replaced: "+str(aImmediateReplacedNameList))

				#===== 1D ENTRIES - ARRAYS, index of array is an infotype ENUM =================================================================#
				#<YieldChanges>, <YieldPerPopChanges>, <SeaPlotYieldChanges>, <RiverPlotYieldChanges>, <YieldModifiers>, <PowerYieldModifiers>, <AreaYieldModifiers>, <GlobalYieldModifiers> - base
				aBaseYieldChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aBaseYieldPerPopChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aBaseSeaPlotYieldChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aBaseRiverPlotYieldChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aBaseYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				aBasePowerYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				aBaseAreaYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				aBaseGlobalYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
					aBaseYieldChangesList[iYield] += CvBuildingInfo.getYieldChange(iYield)
					aBaseYieldPerPopChangesList[iYield] += CvBuildingInfo.getYieldPerPopChange(iYield)
					aBaseSeaPlotYieldChangesList[iYield] += CvBuildingInfo.getSeaPlotYieldChange(iYield)
					aBaseRiverPlotYieldChangesList[iYield] += CvBuildingInfo.getRiverPlotYieldChange(iYield)
					aBaseYieldModifiersList[iYield] += CvBuildingInfo.getYieldModifier(iYield)
					aBasePowerYieldModifiersList[iYield] += CvBuildingInfo.getPowerYieldModifier(iYield)
					aBaseAreaYieldModifiersList[iYield] += CvBuildingInfo.getAreaYieldModifier(iYield)
					aBaseGlobalYieldModifiersList[iYield] += CvBuildingInfo.getGlobalYieldModifier(iYield)

				#Analyze replacements by tag
				aYieldChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aYieldPerPopChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aSeaPlotYieldChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aRiverPlotYieldChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				aPowerYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				aAreaYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				aGlobalYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<YieldChanges>, <YieldPerPopChanges>, <SeaPlotYieldChanges>, <RiverPlotYieldChanges>, <YieldModifiers>, <PowerYieldModifiers>, <AreaYieldModifiers>, <GlobalYieldModifiers>
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						aYieldChangesList[iYield] += CvReplacedBuildingInfo.getYieldChange(iYield)
						aYieldPerPopChangesList[iYield] += CvReplacedBuildingInfo.getYieldPerPopChange(iYield)
						aSeaPlotYieldChangesList[iYield] += CvReplacedBuildingInfo.getSeaPlotYieldChange(iYield)
						aRiverPlotYieldChangesList[iYield] += CvReplacedBuildingInfo.getRiverPlotYieldChange(iYield)
						aYieldModifiersList[iYield] += CvReplacedBuildingInfo.getYieldModifier(iYield)
						aPowerYieldModifiersList[iYield] += CvReplacedBuildingInfo.getPowerYieldModifier(iYield)
						aAreaYieldModifiersList[iYield] += CvReplacedBuildingInfo.getAreaYieldModifier(iYield)
						aGlobalYieldModifiersList[iYield] += CvReplacedBuildingInfo.getGlobalYieldModifier(iYield)

				#Keep already existing <YieldChanges>, <YieldPerPopChanges>, <SeaPlotYieldChanges>, <RiverPlotYieldChanges>, <YieldModifiers>, <PowerYieldModifiers>, <AreaYieldModifiers>, <GlobalYieldModifiers> in base
				aFinalYieldChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aFinalYieldPerPopChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aFinalSeaPlotYieldChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aFinalRiverPlotYieldChangesList = [0]*YieldTypes.NUM_YIELD_TYPES
				aFinalYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				aFinalPowerYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				aFinalAreaYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				aFinalGlobalYieldModifiersList = [0]*YieldTypes.NUM_YIELD_TYPES
				for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
					aFinalYieldChangesList[iYield] = aBaseYieldChangesList[iYield] + aYieldChangesList[iYield]
					aFinalYieldPerPopChangesList[iYield] = aBaseYieldPerPopChangesList[iYield] + aYieldPerPopChangesList[iYield]
					aFinalSeaPlotYieldChangesList[iYield] = aBaseSeaPlotYieldChangesList[iYield] + aSeaPlotYieldChangesList[iYield]
					aFinalRiverPlotYieldChangesList[iYield] = aBaseRiverPlotYieldChangesList[iYield] + aRiverPlotYieldChangesList[iYield]
					aFinalYieldModifiersList[iYield] = aBaseYieldModifiersList[iYield] + aYieldModifiersList[iYield]
					aFinalPowerYieldModifiersList[iYield] = aBasePowerYieldModifiersList[iYield] + aPowerYieldModifiersList[iYield]
					aFinalAreaYieldModifiersList[iYield] = aBaseAreaYieldModifiersList[iYield] + aAreaYieldModifiersList[iYield]
					aFinalGlobalYieldModifiersList[iYield] = aBaseGlobalYieldModifiersList[iYield] + aGlobalYieldModifiersList[iYield]

				#Building shouldn't be worse than replaced one!
				for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
					if aBaseYieldChangesList[iYield] < aYieldChangesList[iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have F/P/C Changes "+str(aFinalYieldChangesList)+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseYieldPerPopChangesList[iYield] < aYieldPerPopChangesList[iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have F/P/C Per pop Changes "+str(aFinalYieldPerPopChangesList)+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseSeaPlotYieldChangesList[iYield] < aSeaPlotYieldChangesList[iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have F/P/C Sea plot Yield Changes "+str(aFinalSeaPlotYieldChangesList)+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseRiverPlotYieldChangesList[iYield] < aRiverPlotYieldChangesList[iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have F/P/C River plot Yield Changes "+str(aFinalRiverPlotYieldChangesList)+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseYieldModifiersList[iYield] < aYieldModifiersList[iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have F/P/C Modifiers "+str(aFinalYieldModifiersList)+" replaced: "+str(aImmediateReplacedNameList))
					if aBasePowerYieldModifiersList[iYield] < aPowerYieldModifiersList[iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have F/P/C Power Modifiers "+str(aFinalPowerYieldModifiersList)+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseAreaYieldModifiersList[iYield] < aAreaYieldModifiersList[iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have F/P/C Area Modifiers "+str(aFinalAreaYieldModifiersList)+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseGlobalYieldModifiersList[iYield] < aGlobalYieldModifiersList[iYield]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have F/P/C Global Modifiers "+str(aFinalGlobalYieldModifiersList)+" replaced: "+str(aImmediateReplacedNameList))

				#=================================================================================================
				#<CommerceChanges>, <CommercePerPopChanges>, <CommerceModifiers>, <GlobalCommerceModifiers> - base
				aBaseCommerceChanges = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				aBaseCommercePerPopChanges = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				aBaseCommerceModifiers = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				aBaseGlobalCommerceModifiers = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
					aBaseCommerceChanges[iCommerce] += CvBuildingInfo.getCommerceChange(iCommerce)
					aBaseCommercePerPopChanges[iCommerce] += CvBuildingInfo.getCommercePerPopChange(iCommerce)
					aBaseCommerceModifiers[iCommerce] += CvBuildingInfo.getCommerceModifier(iCommerce)
					aBaseGlobalCommerceModifiers[iCommerce] += CvBuildingInfo.getGlobalCommerceModifier(iCommerce)

				#Analyze replacements by tag
				aCommerceChanges = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				aCommercePerPopChanges = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				aCommerceModifiers = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				aGlobalCommerceModifiers = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<CommerceChanges>, <CommercePerPopChanges>, <CommerceModifiers>, <GlobalCommerceModifiers>
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						aCommerceChanges[iCommerce] += CvReplacedBuildingInfo.getCommerceChange(iCommerce)
						aCommercePerPopChanges[iCommerce] += CvReplacedBuildingInfo.getCommercePerPopChange(iCommerce)
						aCommerceModifiers[iCommerce] += CvReplacedBuildingInfo.getCommerceModifier(iCommerce)
						aGlobalCommerceModifiers[iCommerce] += CvReplacedBuildingInfo.getGlobalCommerceModifier(iCommerce)

				#Keep already existing <CommerceChanges>, <CommercePerPopChanges>, <CommerceModifiers>, <GlobalCommerceModifiers> in base
				aFinalCommerceChanges = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				aFinalCommercePerPopChanges = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				aFinalCommerceModifiers = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				aFinalGlobalCommerceModifiers = [0]*CommerceTypes.NUM_COMMERCE_TYPES
				for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
					aFinalCommerceChanges[iCommerce] = aBaseCommerceChanges[iCommerce] + aCommerceChanges[iCommerce]
					aFinalCommercePerPopChanges[iCommerce] = aBaseCommercePerPopChanges[iCommerce] + aCommercePerPopChanges[iCommerce]
					aFinalCommerceModifiers[iCommerce] = aBaseCommerceModifiers[iCommerce] + aCommerceModifiers[iCommerce]
					aFinalGlobalCommerceModifiers[iCommerce] = aBaseGlobalCommerceModifiers[iCommerce] + aGlobalCommerceModifiers[iCommerce]

				#Building shouldn't be worse than replaced one!
				for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
					if aBaseCommerceChanges[iCommerce] < aCommerceChanges[iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have G/R/C/E Changes "+str(aFinalCommerceChanges)+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseCommercePerPopChanges[iCommerce] < aCommercePerPopChanges[iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have G/R/C/E Per pop Changes "+str(aFinalCommercePerPopChanges)+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseCommerceModifiers[iCommerce] < aCommerceModifiers[iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have G/R/C/E Modifiers "+str(aFinalCommerceModifiers)+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseGlobalCommerceModifiers[iCommerce] < aGlobalCommerceModifiers[iCommerce]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have G/R/C/E Global Modifiers "+str(aFinalGlobalCommerceModifiers)+" replaced: "+str(aImmediateReplacedNameList))

				#=================================================================================================
				#<SpecialistCounts>, <FreeSpecialistCounts> - base
				aBaseSpecialistCounts = [0]*GC.getNumSpecialistInfos()
				aBaseFreeSpecialistCounts = [0]*GC.getNumSpecialistInfos()
				for iSpecialist in xrange(GC.getNumSpecialistInfos()):
					aBaseSpecialistCounts[iSpecialist] += CvBuildingInfo.getSpecialistCount(iSpecialist)
					aBaseFreeSpecialistCounts[iSpecialist] += CvBuildingInfo.getFreeSpecialistCount(iSpecialist)

				#Analyze replacements by tag
				aSpecialistCounts = [0]*GC.getNumSpecialistInfos()
				aFreeSpecialistCount = [0]*GC.getNumSpecialistInfos()
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<SpecialistCounts>, <FreeSpecialistCounts>
					for iSpecialist in xrange(GC.getNumSpecialistInfos()):
						aSpecialistCounts[iSpecialist] += CvReplacedBuildingInfo.getSpecialistCount(iSpecialist)
						aFreeSpecialistCount[iSpecialist] += CvReplacedBuildingInfo.getFreeSpecialistCount(iSpecialist)

				#Keep already existing <SpecialistCounts>, <FreeSpecialistCounts> in base
				aFinalSpecialistCounts = [0]*GC.getNumSpecialistInfos()
				aFinalFreeSpecialistCount = [0]*GC.getNumSpecialistInfos()
				for iSpecialist in xrange(GC.getNumSpecialistInfos()):
					aFinalSpecialistCounts[iSpecialist] = aBaseSpecialistCounts[iSpecialist] + aSpecialistCounts[iSpecialist]
					aFinalFreeSpecialistCount[iSpecialist] = aBaseFreeSpecialistCounts[iSpecialist] + aFreeSpecialistCount[iSpecialist]

				#Building shouldn't be worse than replaced one!
				#Emancipation Proclamation removes worldview buildings, python is needed for it to actually erase all slave specialists
				for iSpecialist in xrange(GC.getNumSpecialistInfos()):
					if aBaseSpecialistCounts[iSpecialist] < aSpecialistCounts[iSpecialist] and iBuilding != GC.getInfoTypeForString("BUILDING_EMANCIPATION_PROCLAMATION_EFFECT"):
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getSpecialistInfo(iSpecialist).getType()+" Specialists Count "+str(aFinalSpecialistCounts[iSpecialist])+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseFreeSpecialistCounts[iSpecialist] < aFreeSpecialistCount[iSpecialist] and iBuilding != GC.getInfoTypeForString("BUILDING_EMANCIPATION_PROCLAMATION_EFFECT"):
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getSpecialistInfo(iSpecialist).getType()+" Free specialists Count "+str(aFinalFreeSpecialistCount[iSpecialist])+" replaced: "+str(aImmediateReplacedNameList))

				#=================================================================================================
				#<ImprovementFreeSpecialists> - base
				aBaseImprovementFreeSpecialists = [0]*GC.getNumImprovementInfos()
				for iImprovement in xrange(GC.getNumImprovementInfos()):
					aBaseImprovementFreeSpecialists[iImprovement] += CvBuildingInfo.getImprovementFreeSpecialist(iImprovement)

				#Analyze replacements by tag
				aImprovementFreeSpecialists = [0]*GC.getNumImprovementInfos()
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<ImprovementFreeSpecialists>
					for iImprovement in xrange(GC.getNumImprovementInfos()):
						aImprovementFreeSpecialists[iImprovement] += CvReplacedBuildingInfo.getImprovementFreeSpecialist(iImprovement)

				#Keep already existing <ImprovementFreeSpecialists> in base
				aFinalImprovementFreeSpecialists = [0]*GC.getNumImprovementInfos()
				for iImprovement in xrange(GC.getNumImprovementInfos()):
					aFinalImprovementFreeSpecialists[iImprovement] = aBaseImprovementFreeSpecialists[iImprovement] + aImprovementFreeSpecialists[iImprovement]

				#Building shouldn't be worse than replaced one!
				for iImprovement in xrange(GC.getNumImprovementInfos()):
					if aBaseImprovementFreeSpecialists[iImprovement] < aImprovementFreeSpecialists[iImprovement]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getImprovementInfo(iImprovement).getType()+" Improvement free Specialist "+str(aFinalImprovementFreeSpecialists[iImprovement])+" replaced: "+str(aImmediateReplacedNameList))

				#=================================================================================================
				#<BonusHappinessChanges>, <BonusHealthChanges> - base
				aBaseBonusHappinessChanges = [0]*GC.getNumBonusInfos()
				aBaseBonusHealthChanges = [0]*GC.getNumBonusInfos()
				for iBonus in xrange(GC.getNumBonusInfos()):
					aBaseBonusHappinessChanges[iBonus] += CvBuildingInfo.getBonusHappinessChanges(iBonus)
					aBaseBonusHealthChanges[iBonus] += CvBuildingInfo.getBonusHealthChanges(iBonus)

				#Analyze replacements by tag
				aBonusHappinessChanges = [0]*GC.getNumBonusInfos()
				aBonusHealthChanges = [0]*GC.getNumBonusInfos()
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<BonusHappinessChanges>, <BonusHealthChanges>
					for iBonus in xrange(GC.getNumBonusInfos()):
						aBonusHappinessChanges[iBonus] += CvReplacedBuildingInfo.getBonusHappinessChanges(iBonus)
						aBonusHealthChanges[iBonus] += CvReplacedBuildingInfo.getBonusHealthChanges(iBonus)

				#Keep already existing <BonusHappinessChanges>, <BonusHealthChanges> in base
				aFinalBonusHappinessChanges = [0]*GC.getNumBonusInfos()
				aFinalBonusHealthChanges = [0]*GC.getNumBonusInfos()
				for iBonus in xrange(GC.getNumBonusInfos()):
					aFinalBonusHappinessChanges[iBonus] = aBaseBonusHappinessChanges[iBonus] + aBonusHappinessChanges[iBonus]
					aFinalBonusHealthChanges[iBonus] = aBaseBonusHealthChanges[iBonus] + aBonusHealthChanges[iBonus]

				#Building shouldn't be worse than replaced one!
				for iBonus in xrange(GC.getNumBonusInfos()):
					if aBaseBonusHappinessChanges[iBonus] < aBonusHappinessChanges[iBonus]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" Bonus happiness Changes "+str(aFinalBonusHappinessChanges[iBonus])+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseBonusHealthChanges[iBonus] < aBonusHealthChanges[iBonus]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" Bonus health Changes "+str(aFinalBonusHealthChanges[iBonus])+" replaced: "+str(aImmediateReplacedNameList))

				#=================================================================================================
				#<TechHappinessChanges>, <TechHealthChanges> - base
				aBaseTechHappinessChanges = [0]*GC.getNumTechInfos()
				aBaseTechHealthChanges = [0]*GC.getNumTechInfos()
				for pair in CvBuildingInfo.getTechHappinessChanges():
					aBaseTechHappinessChanges[pair.id] += pair.value
				for pair in CvBuildingInfo.getTechHealthChanges():
					aBaseTechHealthChanges[pair.id] += pair.value

				#Analyze replacements by tag
				aTechHappinessChanges = [0]*GC.getNumTechInfos()
				aTechHealthChanges = [0]*GC.getNumTechInfos()
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<TechHappinessChanges>, <TechHealthChanges>
					for pair in CvReplacedBuildingInfo.getTechHappinessChanges():
						aTechHappinessChanges[pair.id] += pair.value
					for pair in CvReplacedBuildingInfo.getTechHealthChanges():
						aTechHealthChanges[pair.id] += pair.value

				#Keep already existing <TechHappinessChanges>, <TechHealthChanges> in base
				aFinalTechHappinessChanges = [0]*GC.getNumTechInfos()
				aFinalTechHealthChanges = [0]*GC.getNumTechInfos()
				for iTech in xrange(GC.getNumTechInfos()):
					aFinalTechHappinessChanges[iTech] = aBaseTechHappinessChanges[iTech] + aTechHappinessChanges[iTech]
					aFinalTechHealthChanges[iTech] = aBaseTechHealthChanges[iTech] + aTechHealthChanges[iTech]

				#Building shouldn't be worse than replaced one!
				for iTech in xrange(GC.getNumTechInfos()):
					if aBaseTechHappinessChanges[iTech] < aTechHappinessChanges[iTech]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech happiness Changes "+str(aFinalTechHappinessChanges[iTech])+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseTechHealthChanges[iTech] < aTechHealthChanges[iTech]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech health Changes "+str(aFinalTechHealthChanges[iTech])+" replaced: "+str(aImmediateReplacedNameList))

				#=============================================================================================================================
				#<GlobalBuildingCostModifiers>, <GlobalBuildingProductionModifiers>, <BuildingHappinessChanges>, <BuildingProductionModifiers> - base
				aBaseGlobalBuildingCostModifiers = [0]*GC.getNumBuildingInfos()
				aBaseGlobalBuildingProductionModifiers = [0]*GC.getNumBuildingInfos()
				aBaseBuildingHappinessChanges = [0]*GC.getNumBuildingInfos()
				aBaseBuildingProductionModifiers = [0]*GC.getNumBuildingInfos()
				for pair in CvBuildingInfo.getGlobalBuildingCostModifiers():
					aBaseGlobalBuildingCostModifiers[pair.id] += pair.value
				for pair in CvBuildingInfo.getGlobalBuildingProductionModifiers():
					aBaseGlobalBuildingProductionModifiers[pair.id] += pair.value
				for pair in CvBuildingInfo.getBuildingHappinessChanges():
					aBaseBuildingHappinessChanges[pair.id] += pair.value
				for pair in CvBuildingInfo.getBuildingProductionModifiers():
					aBaseBuildingProductionModifiers[pair.id] += pair.value

				#Analyze replacements by tag
				aGlobalBuildingCostModifiers = [0]*GC.getNumBuildingInfos()
				aGlobalBuildingProductionModifiers = [0]*GC.getNumBuildingInfos()
				aBuildingHappinessChanges = [0]*GC.getNumBuildingInfos()
				aBuildingProductionModifiers = [0]*GC.getNumBuildingInfos()
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<GlobalBuildingCostModifiers>, <GlobalBuildingProductionModifiers>, <BuildingHappinessChanges>, <BuildingProductionModifiers>
					for pair in CvReplacedBuildingInfo.getGlobalBuildingCostModifiers():
						aGlobalBuildingCostModifiers[pair.id] += pair.value
					for pair in CvReplacedBuildingInfo.getGlobalBuildingProductionModifiers():
						aGlobalBuildingProductionModifiers[pair.id] += pair.value
					for pair in CvReplacedBuildingInfo.getBuildingHappinessChanges():
						aBuildingHappinessChanges[pair.id] += pair.value
					for pair in CvReplacedBuildingInfo.getBuildingProductionModifiers():
						aBuildingProductionModifiers[pair.id] += pair.value

				#Keep already existing <GlobalBuildingCostModifiers>, <GlobalBuildingProductionModifiers>, <BuildingHappinessChanges>, <BuildingProductionModifiers> in base
				aFinalGlobalBuildingCostModifiers = [0]*GC.getNumBuildingInfos()
				aFinalGlobalBuildingProductionModifiers = [0]*GC.getNumBuildingInfos()
				aFinalBuildingHappinessChanges = [0]*GC.getNumBuildingInfos()
				aFinalBuildingProductionModifiers = [0]*GC.getNumBuildingInfos()
				for iAffectedBuildings in xrange(GC.getNumBuildingInfos()):
					aFinalGlobalBuildingCostModifiers[iAffectedBuildings] = aBaseGlobalBuildingCostModifiers[iAffectedBuildings] + aGlobalBuildingCostModifiers[iAffectedBuildings]
					aFinalGlobalBuildingProductionModifiers[iAffectedBuildings] = aBaseGlobalBuildingProductionModifiers[iAffectedBuildings] + aGlobalBuildingProductionModifiers[iAffectedBuildings]
					aFinalBuildingHappinessChanges[iAffectedBuildings] = aBaseBuildingHappinessChanges[iAffectedBuildings] + aBuildingHappinessChanges[iAffectedBuildings]
					aFinalBuildingProductionModifiers[iAffectedBuildings] = aBaseBuildingProductionModifiers[iAffectedBuildings] + aBuildingProductionModifiers[iAffectedBuildings]

				#Building shouldn't be worse than replaced one!
				for iAffectedBuildings in xrange(GC.getNumBuildingInfos()):
					if aBaseGlobalBuildingCostModifiers[iAffectedBuildings] < aGlobalBuildingCostModifiers[iAffectedBuildings]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuildings).getType()+" Global Building cost Modifiers "+str(aFinalGlobalBuildingCostModifiers[iAffectedBuildings])+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseGlobalBuildingProductionModifiers[iAffectedBuildings] < aGlobalBuildingProductionModifiers[iAffectedBuildings]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuildings).getType()+" Global Building production Modifiers "+str(aFinalGlobalBuildingProductionModifiers[iAffectedBuildings])+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseBuildingHappinessChanges[iAffectedBuildings] < aBuildingHappinessChanges[iAffectedBuildings]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuildings).getType()+" Building Happiness Changes "+str(aFinalBuildingHappinessChanges[iAffectedBuildings])+" replaced: "+str(aImmediateReplacedNameList))
					if aBaseBuildingProductionModifiers[iAffectedBuildings] < aBuildingProductionModifiers[iAffectedBuildings]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuildings).getType()+" Building production Modifiers "+str(aFinalBuildingProductionModifiers[iAffectedBuildings])+" replaced: "+str(aImmediateReplacedNameList))

				#=================================================================================================
				#<DomainFreeExperiences> - base
				aBaseDomainFreeExperiences = [0]*DomainTypes.NUM_DOMAIN_TYPES
				for iDomain in xrange(DomainTypes.NUM_DOMAIN_TYPES):
					aBaseDomainFreeExperiences[iDomain] += CvBuildingInfo.getDomainFreeExperience(iDomain)

				#Analyze replacements by tag
				aDomainFreeExperiences = [0]*DomainTypes.NUM_DOMAIN_TYPES
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<DomainFreeExperiences>
					for iDomain in xrange(DomainTypes.NUM_DOMAIN_TYPES):
						aDomainFreeExperiences[iDomain] += CvReplacedBuildingInfo.getDomainFreeExperience(iDomain)

				#Keep already existing <DomainFreeExperiences> in base
				aFinalDomainFreeExperiences = [0]*DomainTypes.NUM_DOMAIN_TYPES
				for iDomain in xrange(DomainTypes.NUM_DOMAIN_TYPES):
					aFinalDomainFreeExperiences[iDomain] = aBaseDomainFreeExperiences[iDomain] + aDomainFreeExperiences[iDomain]

				#Building shouldn't be worse than replaced one!
				for iDomain in xrange(DomainTypes.NUM_DOMAIN_TYPES):
					if aBaseDomainFreeExperiences[iDomain] < aDomainFreeExperiences[iDomain]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getDomainInfo(iDomain).getType()+" Domain Free Experience "+str(aFinalDomainFreeExperiences[iDomain])+" replaced: "+str(aImmediateReplacedNameList))

				#=================================================================================================
				#<UnitCombatFreeExperiences> - base
				aBaseUnitCombatFreeExperiences = [0]*GC.getNumUnitCombatInfos()
				for iUnitCombat in xrange(GC.getNumUnitCombatInfos()):
					aBaseUnitCombatFreeExperiences[iUnitCombat] += CvBuildingInfo.getUnitCombatFreeExperience(iUnitCombat)

				#Analyze replacements by tag
				aUnitCombatFreeExperiences = [0]*GC.getNumUnitCombatInfos()
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<UnitCombatFreeExperiences>
					for iUnitCombat in xrange(GC.getNumUnitCombatInfos()):
						aUnitCombatFreeExperiences[iUnitCombat] += CvReplacedBuildingInfo.getUnitCombatFreeExperience(iUnitCombat)

				#Keep already existing <UnitCombatFreeExperiences> in base
				aFinalUnitCombatFreeExperiences = [0]*GC.getNumUnitCombatInfos()
				for iUnitCombat in xrange(GC.getNumUnitCombatInfos()):
					aFinalUnitCombatFreeExperiences[iUnitCombat] = aBaseUnitCombatFreeExperiences[iUnitCombat] + aUnitCombatFreeExperiences[iUnitCombat]

				#Building shouldn't be worse than replaced one!
				for iUnitCombat in xrange(GC.getNumUnitCombatInfos()):
					if aBaseUnitCombatFreeExperiences[iUnitCombat] < aUnitCombatFreeExperiences[iUnitCombat]:
						self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getUnitCombatInfo(iUnitCombat).getType()+" Unit Combat free Experience "+str(aFinalUnitCombatFreeExperiences[iUnitCombat])+" replaced: "+str(aImmediateReplacedNameList))

				#===== 2D ENTRIES - coupling between two infotypes, like yield changing in presence of bonus =====#
				#<BonusYieldChanges>, <VicinityBonusYieldChanges>, <BonusYieldModifiers>, <BonusCommercePercentChanges>, <BonusCommerceModifiers> - base
				aBaseBonusYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aBaseVicinityBonusYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aBaseBonusYieldModifiers = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aBaseBonusCommercePercentChanges = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aBaseBonusCommerceModifiers = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				for iBonus in xrange(GC.getNumBonusInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						aBaseBonusYieldChanges[iBonus][iYield] += CvBuildingInfo.getBonusYieldChanges(iBonus, iYield)
						aBaseVicinityBonusYieldChanges[iBonus][iYield] += CvBuildingInfo.getVicinityBonusYieldChanges(iBonus, iYield)
						aBaseBonusYieldModifiers[iBonus][iYield] += CvBuildingInfo.getBonusYieldModifier(iBonus, iYield)
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						aBaseBonusCommercePercentChanges[iBonus][iCommerce] += CvBuildingInfo.getBonusCommercePercentChanges(iBonus, iCommerce)
						aBaseBonusCommerceModifiers[iBonus][iCommerce] += CvBuildingInfo.getBonusCommerceModifier(iBonus, iCommerce)

				#Analyze replacements by tag
				aBonusYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aBonusVicinityBonusYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aBonusYieldModifiers = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aBonusCommercePercentChanges = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aBonusCommerceModifiers = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<BonusYieldChanges>, <VicinityBonusYieldChanges>, <BonusYieldModifiers>, <BonusCommercePercentChanges>, <BonusCommerceModifiers>
					for iBonus in xrange(GC.getNumBonusInfos()):
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							aBonusYieldChanges[iBonus][iYield] += CvReplacedBuildingInfo.getBonusYieldChanges(iBonus, iYield)
							aBonusVicinityBonusYieldChanges[iBonus][iYield] += CvReplacedBuildingInfo.getVicinityBonusYieldChanges(iBonus, iYield)
							aBonusYieldModifiers[iBonus][iYield] += CvReplacedBuildingInfo.getBonusYieldModifier(iBonus, iYield)
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							aBonusCommercePercentChanges[iBonus][iCommerce] += CvReplacedBuildingInfo.getBonusCommercePercentChanges(iBonus, iCommerce)
							aBonusCommerceModifiers[iBonus][iCommerce] += CvReplacedBuildingInfo.getBonusCommerceModifier(iBonus, iCommerce)

				#Keep already existing <BonusYieldChanges>, <VicinityBonusYieldChanges>, <BonusYieldModifiers>, <BonusCommercePercentChanges>, <BonusCommerceModifiers> in base
				aFinalBonusYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aFinalBonusVicinityBonusYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aFinalBonusYieldModifiers = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aFinalBonusCommercePercentChanges = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				aFinalBonusCommerceModifiers = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBonusInfos())]
				for iBonus in xrange(GC.getNumBonusInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						aFinalBonusYieldChanges[iBonus][iYield] = aBaseBonusYieldChanges[iBonus][iYield] + aBonusYieldChanges[iBonus][iYield]
						aFinalBonusVicinityBonusYieldChanges[iBonus][iYield] = aBaseVicinityBonusYieldChanges[iBonus][iYield] + aBonusVicinityBonusYieldChanges[iBonus][iYield]
						aFinalBonusYieldModifiers[iBonus][iYield] = aBaseBonusYieldModifiers[iBonus][iYield] + aBonusYieldModifiers[iBonus][iYield]
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						aFinalBonusCommercePercentChanges[iBonus][iCommerce] = aBaseBonusCommercePercentChanges[iBonus][iCommerce] + aBonusCommercePercentChanges[iBonus][iCommerce]
						aFinalBonusCommerceModifiers[iBonus][iCommerce] = aBaseBonusCommerceModifiers[iBonus][iCommerce] + aBonusCommerceModifiers[iBonus][iCommerce]

				#Building shouldn't be worse than replaced one!
				for iBonus in xrange(GC.getNumBonusInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if aBaseBonusYieldChanges[iBonus][iYield] < aBonusYieldChanges[iBonus][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" Bonus Yield Changes "+str(aFinalBonusYieldChanges[iBonus])+" replaced: "+str(aImmediateReplacedNameList))
						if aBaseVicinityBonusYieldChanges[iBonus][iYield] < aBonusVicinityBonusYieldChanges[iBonus][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" Bonus vicinity Yield Changes "+str(aFinalBonusVicinityBonusYieldChanges[iBonus])+" replaced: "+str(aImmediateReplacedNameList))
						if aBaseBonusYieldModifiers[iBonus][iYield] < aBonusYieldModifiers[iBonus][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" Bonus Yield Modifiers "+str(aFinalBonusYieldModifiers[iBonus])+" replaced: "+str(aImmediateReplacedNameList))
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						if aBaseBonusCommercePercentChanges[iBonus][iCommerce] < aBonusCommercePercentChanges[iBonus][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" Bonus Commerce percent Changes "+str(aFinalBonusCommercePercentChanges[iBonus])+" replaced: "+str(aImmediateReplacedNameList))
						if aBaseBonusCommerceModifiers[iBonus][iCommerce] < aBonusCommerceModifiers[iBonus][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBonusInfo(iBonus).getType()+" Bonus Commerce Modifiers "+str(aFinalBonusCommerceModifiers[iBonus])+" replaced: "+str(aImmediateReplacedNameList))

				#======================================================================================================================================
				#<TechCommerceChanges>, <TechYieldModifiers>, <TechCommerceChanges>, <TechCommerceModifiers>, <TechSpecialistChanges> - base
				aBaseTechYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aBaseTechYieldModifiers = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aBaseTechCommerceChanges = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aBaseTechCommerceModifiers = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aBaseTechSpecialistChanges = [[0 for x in xrange(GC.getNumSpecialistInfos())] for y in xrange(GC.getNumTechInfos())]
				for iTech in xrange(GC.getNumTechInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						aBaseTechYieldChanges[iTech][iYield] += CvBuildingInfo.getTechYieldChange(iTech, iYield)
						aBaseTechYieldModifiers[iTech][iYield] += CvBuildingInfo.getTechYieldModifier(iTech, iYield)
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						aBaseTechCommerceChanges[iTech][iCommerce] += CvBuildingInfo.getTechCommerceChange(iTech, iCommerce)
						aBaseTechCommerceModifiers[iTech][iCommerce] += CvBuildingInfo.getTechCommerceModifier(iTech, iCommerce)
					for iSpecialist in xrange(GC.getNumSpecialistInfos()):
						aBaseTechSpecialistChanges[iTech][iSpecialist] += CvBuildingInfo.getTechSpecialistChange(iTech, iSpecialist)

				#Analyze replacements by tag
				aTechYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aTechYieldModifiers = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aTechCommerceChanges = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aTechCommerceModifiers = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aTechSpecialistChanges = [[0 for x in xrange(GC.getNumSpecialistInfos())] for y in xrange(GC.getNumTechInfos())]
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<TechCommerceChanges>, <TechYieldModifiers>, <TechCommerceChanges>, <TechCommerceModifiers>, <TechSpecialistChanges>
					for iTech in xrange(GC.getNumTechInfos()):
						for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
							aTechYieldChanges[iTech][iYield] += CvReplacedBuildingInfo.getTechYieldChange(iTech, iYield)
							aTechYieldModifiers[iTech][iYield] += CvReplacedBuildingInfo.getTechYieldModifier(iTech, iYield)
						for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							aTechCommerceChanges[iTech][iCommerce] += CvReplacedBuildingInfo.getTechCommerceChange(iTech, iCommerce)
							aTechCommerceModifiers[iTech][iCommerce] += CvReplacedBuildingInfo.getTechCommerceModifier(iTech, iCommerce)
						for iSpecialist in xrange(GC.getNumSpecialistInfos()):
							aTechSpecialistChanges[iTech][iSpecialist] += CvReplacedBuildingInfo.getTechSpecialistChange(iTech, iSpecialist)

				#Keep already existing <TechCommerceChanges>, <TechYieldModifiers>, <TechCommerceChanges>, <TechCommerceModifiers>, <TechSpecialistChanges> in base
				aFinalTechYieldChanges = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aFinalTechYieldModifiers = [[0 for x in xrange(YieldTypes.NUM_YIELD_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aFinalTechCommerceChanges = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aFinalTechCommerceModifiers = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumTechInfos())]
				aFinalTechSpecialistChanges = [[0 for x in xrange(GC.getNumSpecialistInfos())] for y in xrange(GC.getNumTechInfos())]
				for iTech in xrange(GC.getNumTechInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						aFinalTechYieldChanges[iTech][iYield] = aBaseTechYieldChanges[iTech][iYield] + aTechYieldChanges[iTech][iYield]
						aFinalTechYieldModifiers[iTech][iYield] = aBaseTechYieldModifiers[iTech][iYield] + aTechYieldModifiers[iTech][iYield]
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						aFinalTechCommerceChanges[iTech][iCommerce] = aBaseTechCommerceChanges[iTech][iCommerce] + aTechCommerceChanges[iTech][iCommerce]
						aFinalTechCommerceModifiers[iTech][iCommerce] = aBaseTechCommerceModifiers[iTech][iCommerce] + aTechCommerceModifiers[iTech][iCommerce]
					for iSpecialist in xrange(GC.getNumSpecialistInfos()):
						aFinalTechSpecialistChanges[iTech][iSpecialist] = aBaseTechSpecialistChanges[iTech][iSpecialist] + aTechSpecialistChanges[iTech][iSpecialist]

				#Building shouldn't be worse than replaced one!
				for iTech in xrange(GC.getNumTechInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if aBaseTechYieldChanges[iTech][iYield] < aTechYieldChanges[iTech][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech Yield Changes "+str(aFinalTechYieldChanges[iTech])+" replaced: "+str(aImmediateReplacedNameList))
						if aBaseTechYieldModifiers[iTech][iYield] < aTechYieldModifiers[iTech][iYield]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech Yield Modifiers "+str(aFinalTechYieldModifiers[iTech])+" replaced: "+str(aImmediateReplacedNameList))
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						if aBaseTechCommerceChanges[iTech][iCommerce] < aTechCommerceChanges[iTech][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech Commerce Changes "+str(aFinalTechCommerceChanges[iTech])+" replaced: "+str(aImmediateReplacedNameList))
						if aBaseTechCommerceModifiers[iTech][iCommerce] < aTechCommerceModifiers[iTech][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech Commerce Modifiers "+str(aFinalTechCommerceModifiers[iTech])+" replaced: "+str(aImmediateReplacedNameList))
					for iSpecialist in xrange(GC.getNumSpecialistInfos()):
						if aBaseTechSpecialistChanges[iTech][iSpecialist] < aTechSpecialistChanges[iTech][iSpecialist]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getTechInfo(iTech).getType()+" Tech Specialist Changes "+GC.getSpecialistInfo(iSpecialist).getType()+" replaced: "+str(aFinalTechSpecialistChanges))

				#==============================================================================================================
				#<GlobalBuildingExtraCommerces> - base
				aBaseGlobalBuildingExtraCommerces = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBuildingInfos())]
				for pBuildingCommerceChange in CvBuildingInfo.getGlobalBuildingCommerceChanges():
					iAffectedBuilding = pBuildingCommerceChange.eBuilding
					iCommerce = pBuildingCommerceChange.eCommerce
					aBaseGlobalBuildingExtraCommerces[iAffectedBuilding][iCommerce] += pBuildingCommerceChange.value

				#Analyze replacements by tag
				aGlobalBuildingExtraCommerces = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBuildingInfos())]
				for i in xrange(len(aImmediateReplacedList)):
					CvReplacedBuildingInfo = GC.getBuildingInfo(aImmediateReplacedList[i])
					#<GlobalBuildingExtraCommerces>
					for pBuildingCommerceChange in CvReplacedBuildingInfo.getGlobalBuildingCommerceChanges():
						iAffectedBuilding = pBuildingCommerceChange.eBuilding
						iCommerce = pBuildingCommerceChange.eCommerce
						aGlobalBuildingExtraCommerces[iAffectedBuilding][iCommerce] += pBuildingCommerceChange.value

				#Keep already existing <GlobalBuildingExtraCommerces> in base
				aFinalGlobalBuildingExtraCommerces = [[0 for x in xrange(CommerceTypes.NUM_COMMERCE_TYPES)] for y in xrange(GC.getNumBuildingInfos())]
				for iAffectedBuilding in xrange(GC.getNumBuildingInfos()):
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						aFinalGlobalBuildingExtraCommerces[iAffectedBuilding][iCommerce] = aBaseGlobalBuildingExtraCommerces[iAffectedBuilding][iCommerce] + aGlobalBuildingExtraCommerces[iAffectedBuilding][iCommerce]

				#Building shouldn't be worse than replaced one!
				for iAffectedBuilding in xrange(GC.getNumBuildingInfos()):
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						if aBaseGlobalBuildingExtraCommerces[iAffectedBuilding][iCommerce] < aGlobalBuildingExtraCommerces[iAffectedBuilding][iCommerce]:
							self.log(str(iTechID)+" "+CvBuildingInfo.getType()+" should have "+GC.getBuildingInfo(iAffectedBuilding).getType()+" Global Building extra Commerces "+str(aFinalGlobalBuildingExtraCommerces[iAffectedBuilding])+" replaced: "+str(aImmediateReplacedNameList))

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
				if iAffectedBuilding not in aSpecialBuildingsList and CvAffectedBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1:
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
			for pair in CvBuildingInfo.getGlobalBuildingCostModifiers():
				iAffectedBuilding = pair.id
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				aReplacementBuildingsList = []
				if iAffectedBuilding not in aSpecialBuildingsList and CvAffectedBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for pair in CvBuildingInfo.getGlobalBuildingCostModifiers():
					iAffectedBuilding = pair.id
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if len(aReplacementBuildingsList) > 0:
					self.log(CvBuildingInfo.getType()+" GlobalBuildingCostModifiers "+CvAffectedBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

			#<GlobalBuildingProductionModifiers>
			for pair in CvBuildingInfo.getGlobalBuildingProductionModifiers():
				iAffectedBuilding = pair.id
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				aReplacementBuildingsList = []
				if iAffectedBuilding not in aSpecialBuildingsList and CvAffectedBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for pair in CvBuildingInfo.getGlobalBuildingProductionModifiers():
					iAffectedBuilding = pair.id
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if len(aReplacementBuildingsList) > 0:
					self.log(CvBuildingInfo.getType()+" GlobalBuildingProductionModifiers "+CvAffectedBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

			#<BuildingHappinessChanges>
			for pair in CvBuildingInfo.getBuildingHappinessChanges():
				iAffectedBuilding = pair.id
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				aReplacementBuildingsList = []
				if iAffectedBuilding not in aSpecialBuildingsList and CvAffectedBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for pair in CvBuildingInfo.getBuildingHappinessChanges():
					iAffectedBuilding = pair.id
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if len(aReplacementBuildingsList) > 0:
					self.log(CvBuildingInfo.getType()+" BuildingHappinessChanges "+CvAffectedBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

			#<BuildingProductionModifiers>
			for pair in CvBuildingInfo.getBuildingProductionModifiers():
				iAffectedBuilding = pair.id
				CvAffectedBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
				aReplacementBuildingsList = []
				if iAffectedBuilding not in aSpecialBuildingsList and CvAffectedBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1:
					for i in xrange(CvAffectedBuildingInfo.getNumReplacementBuilding()):
						if CvAffectedBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
							aReplacementBuildingsList.append(GC.getBuildingInfo(CvAffectedBuildingInfo.getReplacementBuilding(i)).getType())
				#If affected building replacement is listed, then remove it from buildings to be added
				for pair in CvBuildingInfo.getBuildingProductionModifiers():
					iAffectedBuilding = pair.id
					if GC.getBuildingInfo(iAffectedBuilding).getType() in aReplacementBuildingsList:
						aReplacementBuildingsList.remove(GC.getBuildingInfo(iAffectedBuilding).getType())
				if len(aReplacementBuildingsList) > 0:
					self.log(CvBuildingInfo.getType()+" BuildingProductionModifiers "+CvAffectedBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

		#Civics referencing buildings
		if 0:# There is currently no way to filter out already existing buildings
			for iCivic in xrange(GC.getNumCivicInfos()):
				CvCivicInfo = GC.getCivicInfo(iCivic)

				#<BuildingHappinessChanges>
				for iAffectedBuilding in xrange(GC.getNumBuildingInfos()):
					CvBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					if CvCivicInfo.getBuildingHappinessChanges(iAffectedBuilding) != 0:
						aReplacementBuildingsList = []
						if iAffectedBuilding not in aSpecialBuildingsList and CvBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1:
							for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
								if CvBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
									aReplacementBuildingsList.append(GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(i)).getType())
						if len(aReplacementBuildingsList) > 0:
							self.log(CvCivicInfo.getType()+" BuildingHappinessChanges "+CvBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

				#<BuildingHealthChanges>
				for iAffectedBuilding in xrange(GC.getNumBuildingInfos()):
					CvBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					if CvCivicInfo.getBuildingHealthChanges(iAffectedBuilding) != 0:
						aReplacementBuildingsList = []
						if iAffectedBuilding not in aSpecialBuildingsList and CvBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1:
							for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
								if CvBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
									aReplacementBuildingsList.append(GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(i)).getType())
						if len(aReplacementBuildingsList) > 0:
							self.log(CvCivicInfo.getType()+" BuildingHealthChanges "+CvBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

				#<BuildingProductionModifiers>
				for iAffectedBuilding in xrange(GC.getNumBuildingInfos()):
					CvBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					if CvCivicInfo.getBuildingProductionModifier(iAffectedBuilding) != 0:
						aReplacementBuildingsList = []
						if iAffectedBuilding not in aSpecialBuildingsList and CvBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1:
							for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
								if CvBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
									aReplacementBuildingsList.append(GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(i)).getType())
						if len(aReplacementBuildingsList) > 0:
							self.log(CvCivicInfo.getType()+" BuildingProductionModifiers "+CvBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

				#<BuildingCommerceModifiers>
				for iAffectedBuilding in xrange(GC.getNumBuildingInfos()):
					CvBuildingInfo = GC.getBuildingInfo(iAffectedBuilding)
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						if CvCivicInfo.getBuildingCommerceModifier(iAffectedBuilding, iCommerce) != 0:
							aReplacementBuildingsList = []
							if iAffectedBuilding not in aSpecialBuildingsList and CvBuildingInfo.getType().find("_STORIES_EFFECT", -15) == -1:
								for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
									if CvBuildingInfo.getReplacementBuilding(i) not in aSpecialBuildingsList: #Get Replacement buildings
										aReplacementBuildingsList.append(GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(i)).getType())
							if len(aReplacementBuildingsList) > 0:
								self.log(CvCivicInfo.getType()+" BuildingCommerceModifiers "+CvBuildingInfo.getType()+" -> "+str(aReplacementBuildingsList))

	#Building bonus requirements
	def checkBuildingBonusRequirements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirements(CvBuildingInfo)[0]
			#<Bonus>BONUS_X
			iBonus = CvBuildingInfo.getPrereqAndBonus()
			if iBonus != -1:
				bonusTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2]
				if bonusTechLoc > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - Singular AND bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<VicinityBonus>BONUS_X
			iBonus = CvBuildingInfo.getPrereqVicinityBonus()
			if iBonus != -1:
				bonusTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2]
				if bonusTechLoc > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - Singular AND vicinity bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<RawVicinityBonus>BONUS_X
			iBonus = CvBuildingInfo.getPrereqRawVicinityBonus()
			if iBonus != -1:
				bonusTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[0]
				if bonusTechLoc > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - Singular AND raw vicinity bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<PrereqBonuses>
			aBonusTechLocList = []
			for iBonus in CvBuildingInfo.getPrereqOrBonuses():
				aBonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - Earliest OR bonus prereq late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#<PrereqVicinityBonuses>
			aBonusTechLocList = []
			for iBonus in CvBuildingInfo.getPrereqOrVicinityBonuses():
				aBonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - Earliest OR vicinity bonus prereq late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#<PrereqRawVicinityBonuses>
			aBonusTechLocList = []
			for iBonus in CvBuildingInfo.getPrereqOrRawVicinityBonuses():
				aBonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[0])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - Earliest OR raw vicinity bonus prereq late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#<ConstructCondition>
			aBonusGOMReqList = []
			for i in range(2):
				aBonusGOMReqList.append([])
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BONUS, aBonusGOMReqList)

			#Analyze GOM AND Bonus reqs
			aBonusTechLocList = []
			for iBonus in xrange(len(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aBonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBonus]))[2])
			if len(aBonusTechLocList) > 0 and max(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - GOM AND bonus requirements are late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Bonus reqs
			aBonusTechLocList = []
			for iBonus in xrange(len(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aBonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBonus]))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - GOM OR bonus requirements are late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

	#Building earliest manufacturer on resource tech reveal
	def checkBuildingBonusManufacturerTech(self):
		#Index of array - Bonus ID, array value at index - producers highest tech requirement location
		aBonusList = [-1]*GC.getNumBonusInfos()
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirements(CvBuildingInfo)[0]

			#Singular <FreeBonus>
			iBonus = CvBuildingInfo.getFreeBonus()
			if iBonus != -1 and CvBuildingInfo.getType().find("_NATURAL_WONDER_") == -1 and iTechLoc != 0: #Natural wonder giving bonus is secondary effect - ignore natural wonders
				if aBonusList[iBonus] == -1:
					aBonusList[iBonus] = iTechLoc
				elif aBonusList[iBonus] != -1 and aBonusList[iBonus] > iTechLoc:
					aBonusList[iBonus] = iTechLoc

			#<ExtraFreeBonuses>
			if CvBuildingInfo.getType().find("_NATURAL_WONDER_") == -1 and iTechLoc != 0: #Ignore producers without tech requirements - those are subdued animal rewards most commonly
				for iBuildingProducer in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
					iBonus = CvBuildingInfo.getExtraFreeBonus(iBuildingProducer)
					if aBonusList[iBonus] == -1:
						aBonusList[iBonus] = iTechLoc
					elif aBonusList[iBonus] != -1 and aBonusList[iBonus] > iTechLoc:
						aBonusList[iBonus] = iTechLoc

		for iBonus in xrange(len(aBonusList)):
			iBonusTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2]
			if aBonusList[iBonus] != -1 and GC.getBonusInfo(iBonus).getTechCityTrade() != -1 and not GC.getBonusInfo(iBonus).getConstAppearance() > 0:
				if aBonusList[iBonus] - iBonusTechLoc != 0:
					self.log(GC.getBonusInfo(iBonus).getType()+" "+str(iBonusTechLoc)+" Earliest bonus producer located at: "+str(aBonusList[iBonus]))

	#Building requirement civic requirements
	def checkBuildingRequirementCivics(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			aBuildingRequirementList = []

			#<PrereqInCityBuildings> - require all buildings in list
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
				aBuildingRequirementList.append(iPrereqBuilding)

			#<PrereqOrBuildings> - require one building in list
			for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuildingRequirement)
				if iPrereqBuilding not in aBuildingRequirementList:
					aBuildingRequirementList.append(iPrereqBuilding)

			#<PrereqAmountBuildings> - require all buildings in empire in list
			for pair in CvBuildingInfo.getPrereqNumOfBuildings():
				iPrereqBuilding = pair.id
				if iPrereqBuilding not in aBuildingRequirementList:
					aBuildingRequirementList.append(iPrereqBuilding)

			#<ConstructCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuildingRequirement]
				if iPrereqBuilding not in aBuildingRequirementList:
					aBuildingRequirementList.append(iPrereqBuilding)

			#Analyze GOM OR Building reqs
			for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuildingRequirement]
				if iPrereqBuilding not in aBuildingRequirementList:
					aBuildingRequirementList.append(iPrereqBuilding)

			#Find if requirement needs civics
			for i in xrange(len(aBuildingRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingRequirementList[i])
				aCivicList = []
				for iCivic in xrange(GC.getNumCivicInfos()):
					if CvBuildingRequirementInfo.isPrereqOrCivics(iCivic) or CvBuildingRequirementInfo.isPrereqAndCivics(iCivic):
						aCivicList.append(GC.getCivicInfo(iCivic).getType())
				if len(aCivicList) > 0:
					self.log(CvBuildingInfo.getType()+" requirement "+CvBuildingRequirementInfo.getType()+" needs civics "+str(aCivicList))

	#Building - civic requirements
	def checkBuildingCivicRequirements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirements(CvBuildingInfo)[0]

			aCivicAndTechLocList = []
			aCivicOrTechLocList = []
			for iCivic in xrange(GC.getNumCivicInfos()):
				#<PrereqAndCivics> - require all civics in list
				if CvBuildingInfo.isPrereqAndCivics(iCivic):
					iCivicTechLoc = self.checkCivicTechRequirementLocation(GC.getCivicInfo(iCivic))[0]
					aCivicAndTechLocList.append(iCivicTechLoc)

				#<PrereqOrCivics> - require one civics in list
				if CvBuildingInfo.isPrereqOrCivics(iCivic):
					iCivicTechLoc = self.checkCivicTechRequirementLocation(GC.getCivicInfo(iCivic))[0]
					aCivicOrTechLocList.append(iCivicTechLoc)

			if len(aCivicAndTechLocList) > 0 and max(aCivicAndTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" is unlocked before its civic AND requirements "+str(aCivicAndTechLocList)+" "+str(iTechLoc))
			if len(aCivicOrTechLocList) > 0 and min(aCivicOrTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" is unlocked before its earliest OR civic requirement "+str(aCivicOrTechLocList)+" "+str(iTechLoc))

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

			#Owner change - ensure flat commerce change exists
			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.isCommerceChangeOriginalOwner(iCommerce) and CvBuildingInfo.getCommerceChange(iCommerce) == 0:
					self.log(CvBuildingInfo.getType()+" has CommerceChangeOriginalOwners but no flat commerce change")

			#Hurry modifiers works only on buildable buildings
			if CvBuildingInfo.getProductionCost() == -1 and (CvBuildingInfo.getHurryCostModifier() != 0 or CvBuildingInfo.getHurryAngerModifier() != 0):
				self.log(CvBuildingInfo.getType()+" can't be hurried at first place")

			#GreatPeopleRateChange must be present, if GreatPeopleUnitType is present
			if CvBuildingInfo.getGreatPeopleUnitType() != -1 and CvBuildingInfo.getGreatPeopleRateChange() == 0:
				self.log(CvBuildingInfo.getType()+" has zero rate change of "+GC.getUnitInfo(CvBuildingInfo.getGreatPeopleUnitType()).getType())

			#FreeBonus must be present if iNumFreeBonuses is present and viceversa.
			if CvBuildingInfo.getFreeBonus() != -1 and CvBuildingInfo.getNumFreeBonuses() == 0:
				self.log(CvBuildingInfo.getType()+" has free bonus tag, but actually doesn't give bonus")
			if CvBuildingInfo.getFreeBonus() == -1 and CvBuildingInfo.getNumFreeBonuses() != 0:
				self.log(CvBuildingInfo.getType()+" has unspecified free bonus type")
			for i in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
				if CvBuildingInfo.getExtraFreeBonus(i) != -1 and CvBuildingInfo.getExtraFreeBonusNum(i) == 0:
					self.log(CvBuildingInfo.getType()+" has extra free bonus tag, but actually doesn't give bonus")
				if CvBuildingInfo.getExtraFreeBonus(i) == -1 and CvBuildingInfo.getExtraFreeBonusNum(i) != 0:
					self.log(CvBuildingInfo.getType()+" has unspecified extra free bonus type")

			#GlobalBuildingExtraCommerces is meant to be placed on wonders
			for pBuildingCommerceChange in CvBuildingInfo.getGlobalBuildingCommerceChanges():
				CvTargetBuilding = GC.getBuildingInfo(pBuildingCommerceChange.eBuilding)
				if not (isWorldWonder(iBuilding) or isNationalWonder(iBuilding) or CvBuildingInfo.getHolyCity() != -1):
					self.log(CvBuildingInfo.getType()+" GlobalBuildingExtraCommerces should be swapped with "+CvTargetBuilding.getType())

	#Buildings - free rewards. Their obsoletion should be correlated with obsoletion of building.
	def checkBuildingFreeReward(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iObsoleteTechLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]

			iFreeBuilding = CvBuildingInfo.getFreeBuilding()
			if iFreeBuilding != -1:
				CvFreeBuilding = GC.getBuildingInfo(iFreeBuilding)
				iObsoleteFreeBuildingTechLoc = self.checkBuildingTechObsoletionLocation(CvFreeBuilding)[0]
				if iObsoleteTechLoc != iObsoleteFreeBuildingTechLoc and CvFreeBuilding.getProductionCost() == -1 or iObsoleteTechLoc < iObsoleteFreeBuildingTechLoc:
					self.log(CvBuildingInfo.getType()+" obsoletes at: "+str(iObsoleteTechLoc)+", free building "+CvFreeBuilding.getType()+" obsoletes at: "+str(iObsoleteFreeBuildingTechLoc))

			iFreeAreaBuilding = CvBuildingInfo.getFreeAreaBuilding()
			if iFreeAreaBuilding != -1:
				CvFreeAreaBuilding = GC.getBuildingInfo(iFreeAreaBuilding)
				iObsoleteFreeAreaBuildingTechLoc = self.checkBuildingTechObsoletionLocation(CvFreeAreaBuilding)[0]
				if iObsoleteTechLoc != iObsoleteFreeAreaBuildingTechLoc and CvFreeAreaBuilding.getProductionCost() == -1 or iObsoleteTechLoc < iObsoleteFreeAreaBuildingTechLoc:
					self.log(CvBuildingInfo.getType()+" obsoletes at: "+str(iObsoleteTechLoc)+", free area building "+CvFreeAreaBuilding.getType()+" obsoletes at: "+str(iObsoleteFreeAreaBuildingTechLoc))

	#Building tech changes and modifiers
	def checkBuildingTechMods(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirements(CvBuildingInfo)[0]
			#Check if Happiness Changes techs don't appear before building can be unlocked or after is obsoleted
			for pair in CvBuildingInfo.getTechHappinessChanges():
				iTech = pair.id
				iTechTLoc = GC.getTechInfo(iTech).getGridX()
				if iTechTLoc <= iTechLoc:
					self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Happiness Changes early tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType())
				elif CvBuildingInfo.getObsoleteTech() != -1 and iTechTLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
					self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Happiness Changes late tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType())

			#Check if Health Changes techs don't appear before building can be unlocked or after is obsoleted
			for pair in CvBuildingInfo.getTechHealthChanges():
				iTech = pair.id
				iTechTLoc = GC.getTechInfo(iTech).getGridX()
				if iTechTLoc <= iTechLoc:
					self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Health Changes early tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType())
				elif CvBuildingInfo.getObsoleteTech() != -1 and iTechTLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
					self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Health Changes late tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType())

			#Check if Yield Changes techs don't appear before building can be unlocked or after is obsoleted
			if CvBuildingInfo.isAnyTechYieldChanges():
				for iTech in xrange(GC.getNumTechInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if CvBuildingInfo.getTechYieldChange(iTech, iYield):
							iTechMLoc = GC.getTechInfo(iTech).getGridX()
							if iTechMLoc <= iTechLoc:
								self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Yield Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Yield Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())

			#Check if Yield Modifiers techs don't appear before building can be unlocked or after is obsoleted
			if CvBuildingInfo.isAnyTechYieldModifiers():
				for iTech in xrange(GC.getNumTechInfos()):
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if CvBuildingInfo.getTechYieldModifier(iTech, iYield):
							iTechMLoc = GC.getTechInfo(iTech).getGridX()
							if iTechMLoc <= iTechLoc:
								self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Yield Modifiers early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Yield Modifiers late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())

			#Check if Commerce Changes techs don't appear before building can be unlocked or after is obsoleted
			if CvBuildingInfo.isAnyTechCommerceChanges():
				for iTech in xrange(GC.getNumTechInfos()):
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						if CvBuildingInfo.getTechCommerceChange(iTech, iCommerce):
							iTechMLoc = GC.getTechInfo(iTech).getGridX()
							if iTechMLoc <= iTechLoc:
								self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Commerce Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Commerce Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTech).getType())

			#Check if Commerce Modifiers techs don't appear before building can be unlocked or after is obsoleted
			if CvBuildingInfo.isAnyTechCommerceModifiers():
				for iTech in xrange(GC.getNumTechInfos()):
					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						if CvBuildingInfo.getTechCommerceModifier(iTech, iCommerce):
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
			iBuildingObsoleteTechLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
			for iBonus in xrange(GC.getNumBonusInfos()):
				CvBonusInfo = GC.getBonusInfo(iBonus)
				iBonusTechEnable = self.checkBonusTechRequirementLocation(CvBonusInfo)[2]
				if iBuildingObsoleteTechLoc < iBonusTechEnable:
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						#<BonusYieldChanges>
						if CvBuildingInfo.getBonusYieldChanges(iBonus, iYield) != 0:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusYieldChanges")

						#<BonusYieldModifiers>
						if CvBuildingInfo.getBonusYieldModifier(iBonus, iYield) != 0:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusYieldModifiers")

						#<VicinityBonusYieldChanges>
						if CvBuildingInfo.getVicinityBonusYieldChanges(iBonus, iYield) != 0:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - VicinityBonusYieldChanges")

					for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						#<BonusCommerceModifiers>
						if CvBuildingInfo.getBonusCommerceModifier(iBonus, iCommerce) != 0:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusCommerceModifiers")

						#<BonusCommercePercentChanges>
						if CvBuildingInfo.getBonusCommercePercentChanges(iBonus, iCommerce) != 0:
							self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - getBonusCommercePercentChanges")

					#<BonusHappinessChanges>
					if CvBuildingInfo.getBonusHappinessChanges(iBonus) != 0:
						self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusHappinessChanges")

					#<BonusHealthChanges>
					if CvBuildingInfo.getBonusHealthChanges(iBonus) != 0:
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
			aAffectedBuildingTechUnlockList.append(self.checkBuildingTechRequirements(CvBuildingInfo)[0])
			aAffectedBuildingTechObsoletionList.append(self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0])

		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvAffectingBuildingInfo = GC.getBuildingInfo(iBuilding)
			iAffectingBuildingUnlockTechLoc = self.checkBuildingTechRequirements(CvAffectingBuildingInfo)[0]
			iAffectingBuildingObsoleteTechLoc = self.checkBuildingTechObsoletionLocation(CvAffectingBuildingInfo)[0]

			#<GlobalBuildingExtraCommerces>
			for pBuildingCommerceChange in CvAffectingBuildingInfo.getGlobalBuildingCommerceChanges():
				i = pBuildingCommerceChange.eBuilding
				if iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[i] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[i]:
					CvAffectedBuildingInfo = GC.getBuildingInfo(i)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - GlobalBuildingExtraCommerces")

			#<GlobalBuildingCostModifiers>
			for pair in CvAffectingBuildingInfo.getGlobalBuildingCostModifiers():
				if pair.value != 0 and (iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[pair.id] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[pair.id]):
					CvAffectedBuildingInfo = GC.getBuildingInfo(pair.id)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - GlobalBuildingProductionModifiers")

			#<GlobalBuildingProductionModifiers>
			for pair in CvAffectingBuildingInfo.getGlobalBuildingProductionModifiers():
				if pair.value != 0 and (iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[pair.id] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[pair.id]):
					CvAffectedBuildingInfo = GC.getBuildingInfo(pair.id)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - GlobalBuildingProductionModifiers")

			#<BuildingHappinessChanges>
			for pair in CvAffectingBuildingInfo.getBuildingHappinessChanges():
				if pair.value != 0 and (iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[pair.id] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[pair.id]):
					CvAffectedBuildingInfo = GC.getBuildingInfo(pair.id)
					self.log(CvAffectingBuildingInfo.getType()+" can't affect "+CvAffectedBuildingInfo.getType()+" as buildings have disjointed tech ranges - BuildingHappinessChanges")

			#<BuildingProductionModifiers>
			for pair in CvAffectingBuildingInfo.getBuildingProductionModifiers():
				if pair.value != 0 and (iAffectingBuildingObsoleteTechLoc < aAffectedBuildingTechUnlockList[pair.id] or iAffectingBuildingUnlockTechLoc > aAffectedBuildingTechObsoletionList[pair.id]):
					CvAffectedBuildingInfo = GC.getBuildingInfo(pair.id)
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

	#Building - check if building doesn't obsolete before civic is available
	def checkBuildingCivicInfluences(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iBuildingObsoleteTechLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
			for iCivic in xrange(GC.getNumCivicInfos()):
				CvCivicInfo = GC.getCivicInfo(iCivic)
				iCivicTechUnlock = self.checkCivicTechRequirementLocation(CvCivicInfo)[0]
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

	#Unit - check unit upgrades
	def checkUnitUpgrades(self):
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			iTechLoc = self.checkUnitTechRequirementLocation(CvUnitInfo)[0]

			if CvUnitInfo.getNumUnitUpgrades() > 0:
				iCost = CvUnitInfo.getProductionCost()
				for iUnitUpgrade in xrange(CvUnitInfo.getNumUnitUpgrades()):
					upgradedDesc = GC.getUnitInfo(CvUnitInfo.getUnitUpgrade(iUnitUpgrade)).getType()
					upgradedCost = GC.getUnitInfo(CvUnitInfo.getUnitUpgrade(iUnitUpgrade)).getProductionCost()
					upgradedTechLoc = self.checkUnitTechRequirementLocation(GC.getUnitInfo(CvUnitInfo.getUnitUpgrade(iUnitUpgrade)))[0]

					dist = upgradedTechLoc - iTechLoc
					costdiff = upgradedCost - iCost

					upgradedUnit = GC.getUnitInfo(CvUnitInfo.getUnitUpgrade(iUnitUpgrade))
					aSecondUpgradeList = []
					for iUnitUpgrade2 in xrange(upgradedUnit.getNumUnitUpgrades()):
						aSecondUpgradeList.append(GC.getUnitInfo(upgradedUnit.getUnitUpgrade(iUnitUpgrade2)).getType())
					if CvUnitInfo.getNumUnitUpgrades() == 1 and (dist <= 2 or costdiff <= 0):
						self.log(str(iTechLoc)+" - "+str(CvUnitInfo.getType())+"; Upgrade: "+str(upgradedTechLoc)+" - "+str(upgradedDesc)+" -> Distance: "+str(dist)+", Cost difference: "+str(costdiff)+" Upgrade of upgrade "+str(aSecondUpgradeList))
					elif CvUnitInfo.getNumUnitUpgrades() > 1 and (dist <= 2 or costdiff <= 0):
						self.log(str(iTechLoc)+" - "+str(CvUnitInfo.getType())+"; Upgrade #"+str(iUnitUpgrade+1)+"/"+str(CvUnitInfo.getNumUnitUpgrades())+": "+str(upgradedTechLoc)+" - "+str(upgradedDesc)+" -> Distance: "+str(dist)+", Cost difference: "+str(costdiff)+" Upgrade of upgrade "+str(aSecondUpgradeList))

	#Unit - check unit bonus requirements
	def checkUnitBonusRequirements(self):
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			iTechLoc = self.checkUnitTechRequirementLocation(CvUnitInfo)[0]

			#<BonusType>BONUS_X
			iBonus = CvUnitInfo.getPrereqAndBonus()
			if iBonus != -1:
				bonusTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2]
				if bonusTechLoc > iTechLoc:
					self.log(CvUnitInfo.getType()+" - Singular AND bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<PrereqBonuses>
			aBonusTechLocList = []
			for iBonus in CvUnitInfo.getPrereqOrBonuses():
				aBonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBonus))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvUnitInfo.getType()+" - Earliest OR bonus prereq late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#<TrainCondition>
			aBonusGOMReqList = []
			for i in range(2):
				aBonusGOMReqList.append([])
			self.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BONUS, aBonusGOMReqList)

			#Analyze GOM AND Bonus reqs
			aBonusTechLocList = []
			for iBonus in xrange(len(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aBonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBonus]))[2])
			if len(aBonusTechLocList) > 0 and max(aBonusTechLocList) > iTechLoc:
				self.log(CvUnitInfo.getType()+" - GOM AND bonus requirements are late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Bonus reqs
			aBonusTechLocList = []
			for iBonus in xrange(len(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aBonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(aBonusGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBonus]))[2])
			if len(aBonusTechLocList) > 0 and min(aBonusTechLocList) > iTechLoc:
				self.log(CvUnitInfo.getType()+" - GOM OR bonus requirements are late! "+str(aBonusTechLocList)+" "+str(iTechLoc))

	#Unit - check building requirements
	def checkUnitRequirements(self):
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			iTechLoc = self.checkUnitTechRequirementLocation(CvUnitInfo)[0]

			#<PrereqAndBuildings> - require all buildings in list
			aBuildingTechLocList = []
			for iBuilding in xrange(CvUnitInfo.getNumPrereqAndBuildings()):
				aBuildingTechLocList.append(self.checkBuildingTechRequirements(GC.getBuildingInfo(CvUnitInfo.getPrereqAndBuilding(iBuilding)))[0])
			if len(aBuildingTechLocList) > 0 and max(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" is unlocked before its AND building requirements "+str(aBuildingTechLocList)+" "+str(iTechLoc))

			#<PrereqOrBuildings> - require one building in list
			aBuildingTechLocList = []
			for iBuilding in xrange(CvUnitInfo.getPrereqOrBuildingsNum()):
				aBuildingTechLocList.append(self.checkBuildingTechRequirements(GC.getBuildingInfo(CvUnitInfo.getPrereqOrBuilding(iBuilding)))[0])
			if len(aBuildingTechLocList) > 0 and min(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" is unlocked before its earliest OR building requirement "+str(aBuildingTechLocList)+" "+str(iTechLoc))

			#<TrainCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aBuildingTechLocList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aBuildingTechLocList.append(self.checkBuildingTechRequirements(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]))[0])
			if len(aBuildingTechLocList) > 0 and max(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" - GOM AND building requirements are late! "+str(aBuildingTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Building reqs
			aBuildingTechLocList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aBuildingTechLocList.append(self.checkBuildingTechRequirements(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]))[0])
			if len(aBuildingTechLocList) > 0 and min(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" - GOM OR building requirements are late! "+str(aBuildingTechLocList)+" "+str(iTechLoc))

	#Unit - check building requirement replacements
	def checkUnitRequirementsReplacements(self):
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)

			#<PrereqAndBuildings> - require all buildings in list
			aBuildingList = []
			for iBuilding in xrange(CvUnitInfo.getNumPrereqAndBuildings()):
				if CvUnitInfo.getPrereqAndBuilding(iBuilding) not in aBuildingList:
					aBuildingList.append(CvUnitInfo.getPrereqAndBuilding(iBuilding))

			#<PrereqOrBuildings> - require one building in list
			for iBuilding in xrange(CvUnitInfo.getPrereqOrBuildingsNum()):
				if CvUnitInfo.getPrereqOrBuilding(iBuilding) not in aBuildingList:
					aBuildingList.append(CvUnitInfo.getPrereqOrBuilding(iBuilding))

			#<TrainCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				if aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding] not in aBuildingList:
					aBuildingList.append(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding])

			#Analyze GOM OR Building reqs
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				if aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding] not in aBuildingList:
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
				self.log(CvUnitInfo.getType()+" requires "+str(aBuildingTypeList))

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
								self.log(CvImprovementInfo.getType()+" with "+CvBonusInfo.getType()+": F/P/C -> "+str(aFinalYield)+" upgrade: "+CvImprovementUpgradeInfo.getType()+": F/P/C -> "+str((aFinalImpUpgradeYield, (aFinalImpUpgradeYield[0]-aFinalYield[0], aFinalImpUpgradeYield[1]-aFinalYield[1], aFinalImpUpgradeYield[2]-aFinalYield[2]))))

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
										self.log(CvImprovementInfo.getType()+" with "+CvBonusInfo.getType()+": F/P/C -> "+str(aFinalYield)+" Alt upgrade: "+CvImprovementAltUpgradeInfo.getType()+": F/P/C -> "+str((aFinalImpAltUpgradeYield, (aFinalImpAltUpgradeYield[0]-aFinalYield[0], aFinalImpAltUpgradeYield[1]-aFinalYield[1], aFinalImpAltUpgradeYield[2]-aFinalYield[2]))))

	#Bonus - check potential bonus producer replacements
	def checkBonusProducerReplacements(self):
		BONUSCLASS_CULTURE = GC.getInfoTypeForString("BONUSCLASS_CULTURE")
		BONUSCLASS_GENMODS = GC.getInfoTypeForString("BONUSCLASS_GENMODS")
		BONUSCLASS_WONDER = GC.getInfoTypeForString("BONUSCLASS_WONDER")

		for iBonus in xrange(GC.getNumBonusInfos()):
			CvBonusInfo = GC.getBonusInfo(iBonus)

			#Check replacements of bonus producers
			if CvBonusInfo.getConstAppearance() == 0 and not (( BONUSCLASS_CULTURE > -1 and CvBonusInfo.getBonusClassType() == BONUSCLASS_CULTURE) or (BONUSCLASS_GENMODS > -1 and CvBonusInfo.getBonusClassType() == BONUSCLASS_GENMODS) or (BONUSCLASS_WONDER > -1 and CvBonusInfo.getBonusClassType() == BONUSCLASS_WONDER)):
			# Check Manufactured bonuses, that aren't Culture or Techno-culture or Unique types.
				aNumBonusManufacturers = [] # Count manufacturers and add their locations
				aBuildingObsoletions = [] # List xgrid of manufacturer tech obsoletions
				for iBuilding in xrange(GC.getNumBuildingInfos()): # Collect statistics about buildings - location of producer and its obsoletion
					CvBuildingInfo = GC.getBuildingInfo(iBuilding)
					if GC.getInfoTypeForString("MAPCATEGORY_EARTH") in CvBuildingInfo.getMapCategories(): # Exclude space based
						if CvBuildingInfo.getFreeBonus() == iBonus:
							aNumBonusManufacturers.append(self.checkBuildingTechRequirements(CvBuildingInfo)[0])
							aBuildingObsoletions.append(self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0])

						for iBonuses in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
							if CvBuildingInfo.getExtraFreeBonus(iBonuses) == iBonus:
								aNumBonusManufacturers.append(self.checkBuildingTechRequirements(CvBuildingInfo)[0])
								aBuildingObsoletions.append(self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0])

				# Check all bonus producers, that don't obsolete
				if len(aNumBonusManufacturers) > 1 and min(aBuildingObsoletions) == 999:
					for iBuilding in xrange(GC.getNumBuildingInfos()):
						CvBuildingInfo = GC.getBuildingInfo(iBuilding)
						if GC.getInfoTypeForString("MAPCATEGORY_EARTH") in CvBuildingInfo.getMapCategories(): # Exclude space based
							aBuildingReplacements = [] # List building replacements
							iObsoleteTechLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
							for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
								CvBuildingReplacement = GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(iReplacement))
								aBuildingReplacements.append(CvBuildingReplacement.getType())

							if CvBuildingInfo.getFreeBonus() == iBonus:
								self.log(CvBonusInfo.getType()+" "+str(self.checkBuildingTechRequirements(CvBuildingInfo)[0])+"/"+str(iObsoleteTechLoc)+" Type: "+CvBuildingInfo.getType()+" Replacement: "+str(aBuildingReplacements))

							for iBonuses in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
								if CvBuildingInfo.getExtraFreeBonus(iBonuses) == iBonus:
									self.log(CvBonusInfo.getType()+" "+str(self.checkBuildingTechRequirements(CvBuildingInfo)[0])+"/"+str(iObsoleteTechLoc)+" Type: "+CvBuildingInfo.getType()+" Replacement: "+str(aBuildingReplacements))

	#Improvement - yield boosts should be between improvement unlock and upgrade
	def checkImprovementTechYieldBoostLocation(self):
		for iImprovement in xrange(GC.getNumImprovementInfos()):
			CvImprovementInfo = GC.getImprovementInfo(iImprovement)
			iTechLoc = self.checkImprovementTechRequirementLocation(CvImprovementInfo)[0]

			if CvImprovementInfo.getImprovementUpgrade() != -1 or CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes() > 0 or CvImprovementInfo.getImprovementPillage() != -1: #Only those, that can upgrade, or are top of upgrade chain
				aTechBoost = []
				CvImprovementUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getImprovementUpgrade())
				iImpUpgradeTechLoc = 0
				if CvImprovementUpgradeInfo != None: # Main upgrade
					iImpUpgradeTechLoc = self.checkImprovementTechRequirementLocation(CvImprovementUpgradeInfo)[0]
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
						iImpAltUpgradeTechLoc = self.checkImprovementTechRequirementLocation(CvImprovementUpgradeInfo)[0]
						if iImpAltUpgradeTechLoc and aTechBoost and iImpAltUpgradeTechLoc <= max(aTechBoost):
							self.log(CvImprovementInfo.getType()+" Xgrid: "+str(iTechLoc)+" Tech boosts location: "+str(aTechBoost)+" Alt Upgrade: "+CvImprovementAltUpgradeInfo.getType()+": "+str(iImpAltUpgradeTechLoc))

	#Improvement - base + tech improvement yields compared to upgraded improvement
	def checkImprovementYieldValues(self):
		for iImprovement in xrange(GC.getNumImprovementInfos()):
			CvImprovementInfo = GC.getImprovementInfo(iImprovement)
			if CvImprovementInfo.getImprovementUpgrade() != -1 or CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes() > 0 or CvImprovementInfo.getImprovementPillage() != -1: #Only those, that can upgrade, or are top of upgrade chain
				iTechLoc = self.checkImprovementTechRequirementLocation(CvImprovementInfo)[0]
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
					self.log(CvImprovementInfo.getType()+" Total F/P/C: "+str(aTotalImprovementYield)+", "+CvImprovementUpgradeInfo.getType()+" Upgrade F/P/C: "+str(aBaseUpgradeImprovementYield))

				for iImprovementUpgrade in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					aBaseAltUpgradeImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
					CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(iImprovementUpgrade))
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						aBaseAltUpgradeImprovementYield[iYield] = CvImprovementAltUpgradeInfo.getYieldChange(iYield)
					if aTotalImprovementYield[0] > aBaseAltUpgradeImprovementYield[0] or aTotalImprovementYield[1] > aBaseAltUpgradeImprovementYield[1] or aTotalImprovementYield[2] > aBaseAltUpgradeImprovementYield[2]:
						self.log(CvImprovementInfo.getType()+" Total F/P/C: "+str(aTotalImprovementYield)+", "+CvImprovementAltUpgradeInfo.getType()+" Alt Upgrade F/P/C: "+str(aBaseAltUpgradeImprovementYield))

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