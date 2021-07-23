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

	# Use this instead of print, it will print to pythonDbg and will also print on screen.
	def log(self, text):
		self.main.testCodelog(text)

	def drawTab(self, screen):
		# Toffer - Add new test code button to the debug screen like this and define the function below.
		#	This is all that is needed for the button to become functional, one line.
		#self.main.addTestCode(screen, functionName, "Button Text", "tooltip")
		self.main.addTestCode(screen, self.listBuildingTechRequirements, "Building tech requirement list", "Creates list of buildings with Main, TechTypes, SpecialBuilding and Religious tech requirements")
		self.main.addTestCode(screen, self.listUnitTechRequirements, "Unit tech requirement list", "Creates list of units with Main and TechTypes tech requirements")
		self.main.addTestCode(screen, self.listTechRequirements, "Tech requirement list", "Creates list of techs with its tech requirements")
		self.main.addTestCode(screen, self.checkBuildingBonusRequirements, "Building bonus requirements", "Checks various bonus prereqs to check if they aren't unlocked after building")
		self.main.addTestCode(screen, self.checkBuildingRequirements, "Building requirements of buildings", "Checks if building requirements aren't unlocked after building itself")
		self.main.addTestCode(screen, self.checkBuildingRequirementReplacements, "Building requirement replacements", "Checks if building requirements are replaced before building itself obsoletes")
		self.main.addTestCode(screen, self.checkBuildingCivicRequirements, "Building - civic requirements", "Checks if various civics aren't unlocked after building")
		self.main.addTestCode(screen, self.checkBuildingRequirementCivics, "Building - requirement civic requirements", "Check if building requirements require civics")
		self.main.addTestCode(screen, self.checkBuildingBonusManufacturerTech, "Building earliest manufacturer on resource tech reveal", "Checks when earliest resource producer is unlocked")
		self.main.addTestCode(screen, self.checkBuildingCommerceDoubleTime, "Building commerce double time", "Checks if commerce double time exists on wonders, that have relevant flat commerce change")
		self.main.addTestCode(screen, self.checkBuildingCommerceChangeOriginalOwners, "Building commerce owner change", "Checks if Commerce Change has relevant flat commerce changes")
		self.main.addTestCode(screen, self.checkBuildingHurryModifier, "Building hurry modifiers", "Checks if hurry modifiers exist on unbuildable buildings")
		self.main.addTestCode(screen, self.checkBuildingUnlockObsoletion, "Buildings unlock/obsoletion", "Checks if building obsoletion doesn't happen within 5 columns of building unlock")
		self.main.addTestCode(screen, self.checkBuildingReplacementObsoletion, "Building obsoletion of replacements", "Checks when replacements are unlocked and obsoleted. Base -> Upgrade: Base tech obsoletion/Upgrade tech unlock, beelining might cause base building to go obsolete before replacement is available, difference of more than 5 columns is assumed safe. Replacing building shouldn't obsolete before replaced one")
		self.main.addTestCode(screen, self.checkBuildingRequirementObsoletion, "Building obsoletion of requirements", "Checks when requirements obsolete in relation to building itself. Building requirements of building shouldn't obsolete before building itself")
		self.main.addTestCode(screen, self.checkBuildingFreeReward, "Building obsoletion of free buildings", "Checks if free buildings - normally unbuildable - obsolete together with building, that gives it for free. Buildable free building shouldn't obsolete before building, that gives it for free")
		self.main.addTestCode(screen, self.checkBuildingWonderMovies, "Building movie wonder list", "Checks movies of noncultural wonders, religious shrines and projects movie location")
		self.main.addTestCode(screen, self.checkBuildingReligionRequirement, "Building religion requirement test", "Checks if tags requiring religion share same religion")
		self.main.addTestCode(screen, self.checkBuildingTechMods, "Building tech changes and modifiers", "Checks if tech modifiers and changes occur within building lifetime")
		self.main.addTestCode(screen, self.checkBuildingBonusTags, "Building - check bonus tags", "Check if bonus tech reveal is after building obsoletion")
		self.main.addTestCode(screen, self.checkBuildingAffectingBuildings, "Building - check building tags", "Check if building affecting other building is within lifetime of each other")
		self.main.addTestCode(screen, self.checkBuildingCivicInfluences, "Building - check civic tags", "Check if building is available when civic is active")
		self.main.addTestCode(screen, self.checkBuildingImplicitReplacements, "Building - check implicit replacements", "Check if we have implicit replacements - All replacements must be explicitly defined even if building got obsoleted long ago")
		self.main.addTestCode(screen, self.checkUnitUpgrades, "Unit - check unit upgrades", "Checks unit upgrades")
		self.main.addTestCode(screen, self.checkUnitBonusRequirements, "Unit - check bonus requirements", "Checks bonus requirements of units")
		self.main.addTestCode(screen, self.checkUnitRequirements, "Unit - check building requirements", "Checks building requirements of units")
		self.main.addTestCode(screen, self.checkBonusImprovementProductivity, "Bonus - check improvement productivity", "Checks if improvement replacements productivity from bonus, improvement and bonus+improvement is higher compared to base improvement")
		self.main.addTestCode(screen, self.checkBonusProducerReplacements, "Bonus - check potential bonus producer replacements", "Checks replacements of manufactured bonus producers")
		self.main.addTestCode(screen, self.checkImprovementTechYieldBoostLocation, "Improvement - yield boost tech requirements", "Checks if yield boosts happen within tech unlock and replacement of improvements")
		self.main.addTestCode(screen, self.checkImprovementYieldValues, "Improvement - all techs boosts compared to upgrade", "Checks if improvement with all tech boosts isn't better than its upgrade")
		self.main.addTestCode(screen, self.listCivics, "Civic - list civics and civic categories", "Creates list civics and civic options")
		self.main.addTestCode(screen, self.listSpaceRequiringEarthBuildings, "Building - List Earth buildings, that require Space content", "Creates list of buildings, that have Earth mapcategory, but have building requirement, that is in space, or is spawned trough space unit/building")

	##### HIGHEST TECH REQUIREMENT LOCATION FINDER FUNCTIONS  #####

	#Building tech location
	def checkBuildingTechRequirementLocation(self, CvBuildingInfo):
		#Main tech requirement
		iTechMainReq = CvBuildingInfo.getPrereqAndTech()
		if iTechMainReq != -1:
			iTechMainLoc = GC.getTechInfo(iTechMainReq).getGridX()
			iTechMainRow = GC.getTechInfo(iTechMainReq).getGridY()
		else:
			iTechMainLoc = 0
			iTechMainRow = 0

		#Tech Type requirement
		aTechTypeLocList = []
		aTechTypeRowList = []
		for iTechType in CvBuildingInfo.getPrereqAndTechs():
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

		#Tech requirement as defined in special building infos (core tech)
		iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()
		if iSpecialBuilding != -1:
			iTechSpecialReq = GC.getSpecialBuildingInfo(iSpecialBuilding).getTechPrereq()
			if iTechSpecialReq != -1:
				iTechSpecialLoc = GC.getTechInfo(iTechSpecialReq).getGridX()
				iTechSpecialRow = GC.getTechInfo(iTechSpecialReq).getGridY()
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
			elif iReligionBuilding == -1:
				iTechReligionLoc = 0
				iTechReligionRow = 0
		else:
			iTechReligionLoc = 0
			iTechReligionRow = 0

		#Tech GOM requirements
		aTechGOMReqList = []
		aTechGOMAndLocList = []
		aTechGOMAndRowList = []
		aTechGOMOrLocList = []
		aTechGOMOrRowList = []
		for i in range(2):
			aTechGOMReqList.append([])
		self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_TECH, aTechGOMReqList)

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
		iTechLoc = max(iTechMainLoc, iTechTypeLoc, iTechSpecialLoc, iTechReligionLoc, iTechGOMAndLoc, iTechGOMOrLoc)
		if iTechLoc == iTechMainLoc:
			iTechRow = iTechMainRow
		if iTechLoc == iTechTypeLoc:
			iTechRow = iTechTypeRow
		elif iTechLoc == iTechSpecialLoc:
			iTechRow = iTechSpecialRow
		elif iTechLoc == iTechReligionLoc:
			iTechRow = iTechReligionRow
		elif iTechLoc == iTechGOMAndLoc:
			iTechRow = iTechGOMAndRow
		elif iTechLoc == iTechGOMOrLoc:
			iTechRow = iTechGOMOrRow

		#This is a Tech location ID - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech requirement, then both infotype X/Y grid is 0
		#Otherwise infotype gets highest Xgrid tech requirement and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		iTechXY = 100*iTechLoc + iTechRow

		return iTechLoc, iTechRow, iTechXY

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

		return iTechLoc, iTechRow, iTechXY

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

		return iTechLoc, iTechRow, iTechXY

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

		return iTechRevealLoc, iTechRevealRow, iTechEnableLoc, iTechEnableRow, iTechRevealXY, iTechEnableXY

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

		return iTechLoc, iTechRow, iTechXY

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

		return iTechLoc, iTechRow, iTechXY

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

		return iTechLoc, iTechRow, iTechXY

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

		return iTechObsLoc, iTechObsXY

	#^^^^ OBSOLETION TECH LOCATION FINDER FUNCTIONS ^^^^#

	#Building tech requirement list
	def listBuildingTechRequirements(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			aTechList = []
			#Main tech requirement
			TechMainReq = CvBuildingInfo.getPrereqAndTech()
			if TechMainReq != -1:
				TechMain = GC.getTechInfo(TechMainReq).getType()
				aTechList.append(TechMain)

			#Tech Type requirement
			for techType in CvBuildingInfo.getPrereqAndTechs():
				aTechList.append(GC.getTechInfo(techType).getType())

			#Tech requirement as defined in special building infos (core tech)
			iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()
			if iSpecialBuilding != -1:
				TechSpecialReq = GC.getSpecialBuildingInfo(iSpecialBuilding).getTechPrereq()
				if TechSpecialReq != -1:
					TechSpecial = GC.getTechInfo(TechSpecialReq).getType()
					aTechList.append(TechSpecial)

			#Tech requirement derived from location of religion in tech tree
			iRelPrereq1 = CvBuildingInfo.getPrereqReligion()
			iRelPrereq2 = CvBuildingInfo.getReligionType()
			iRelPrereq3 = CvBuildingInfo.getPrereqStateReligion()
			if iRelPrereq1 != -1 or iRelPrereq2 != -1 or iRelPrereq3 != -1:
				iReligionBuilding = max(iRelPrereq1, iRelPrereq2, iRelPrereq3)
				if iReligionBuilding != -1:
					TechReligionReq = GC.getReligionInfo(iReligionBuilding).getTechPrereq()
					if TechReligionReq != -1:
						TechReligionLoc = GC.getTechInfo(TechReligionReq).getType()
						aTechList.append(TechReligionLoc)

			#Tech requirement as defined by <ConstructCondition>
			aTechGOMReqList = []
			for j in range(2):
				aTechGOMReqList.append([])
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_TECH, aTechGOMReqList)
			for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aTechList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getType())
			for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aTechList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR][iTech]).getType())

			if len(aTechList) > 0:
				self.log(CvBuildingInfo.getType()+" requires: "+str(aTechList))

	#Unit tech requirement list
	def listUnitTechRequirements(self):
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			aTechList = []
			#Main tech
			TechMainReq = CvUnitInfo.getPrereqAndTech()
			if TechMainReq != -1:
				TechMain = GC.getTechInfo(TechMainReq).getType()
				aTechList.append(TechMain)

			#Tech Type requirement
			for iTechType in CvUnitInfo.getPrereqAndTechs():
				aTechList.append(GC.getTechInfo(iTechType).getType())

			#Tech requirement as defined by <TrainCondition>
			aTechGOMReqList = []
			for i in range(2):
				aTechGOMReqList.append([])
			self.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_TECH, aTechGOMReqList)
			for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				aTechList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getType())
			for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aTechList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR][iTech]).getType())

			if len(aTechList) > 0:
				self.log(CvUnitInfo.getType()+" requires: "+str(aTechList))

	#Tech requirement list
	def listTechRequirements(self):
		for iTech in xrange(GC.getNumTechInfos()):
			CvTechInfo = GC.getTechInfo(iTech)
			aTechList = []
			#AND techs
			for iTechType in CvTechInfo.getPrereqAndTechs():
				aTechList.append(GC.getTechInfo(iTechType).getType())

			if len(CvTechInfo.getPrereqAndTechs()) == 0 and len(CvTechInfo.getPrereqOrTechs()) > 1:
				aTechList.append("one of: ")
			if len(CvTechInfo.getPrereqAndTechs()) > 0 and len(CvTechInfo.getPrereqOrTechs()) > 1:
				aTechList.append(" and one of: ")

			#OR techs
			for iTechType in CvTechInfo.getPrereqOrTechs():
				aTechList.append(GC.getTechInfo(iTechType).getType())

			if len(aTechList) > 0:
				self.log(CvTechInfo.getType()+" requires: "+str(aTechList))

	#Building bonus requirements
	def checkBuildingBonusRequirements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
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

	#Building requirements of buildings
	def checkBuildingRequirements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			#Myth, Story and Stories intentionally doesn't have tech requirements to prevent clogging up space
			if CvBuildingInfo.getType().find("_MYTH") == -1 and CvBuildingInfo.getType().find("_STORY") == -1 and CvBuildingInfo.getType().find("_STORIES") == -1:
				#<PrereqInCityBuildings> - require all buildings in list
				aBuildingTechLocList = []
				for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
					iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
					aBuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				if len(aBuildingTechLocList) > 0 and max(aBuildingTechLocList) > iTechLoc:
					self.log(CvBuildingInfo.getType()+" is unlocked before its AND building requirements "+str(aBuildingTechLocList)+" "+str(iTechLoc))

				#<PrereqOrBuildings> - require one building in list
				aBuildingTechLocList = []
				for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
					iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuildingRequirement)
					aBuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				if len(aBuildingTechLocList) > 0 and min(aBuildingTechLocList) > iTechLoc:
					self.log(CvBuildingInfo.getType()+" is unlocked before its earliest OR building requirement "+str(aBuildingTechLocList)+" "+str(iTechLoc))

				#<PrereqAmountBuildings> - require all buildings in empire in list
				aBuildingTechLocList = []
				for pair in CvBuildingInfo.getPrereqNumOfBuildings():
					iPrereqBuilding = pair.id
					aBuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
				if len(aBuildingTechLocList) > 0 and max(aBuildingTechLocList) > iTechLoc:
					self.log(CvBuildingInfo.getType()+" is unlocked before its Empire AND requirement "+str(aBuildingTechLocList)+" "+str(iTechLoc))

				#<ConstructCondition>
				aBuildingGOMReqList = []
				for i in range(2):
					aBuildingGOMReqList.append([])
				self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

				#Analyze GOM AND Building reqs
				aBuildingTechLocList = []
				for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
					aBuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuildingRequirement]))[0])
				if len(aBuildingTechLocList) > 0 and max(aBuildingTechLocList) > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - GOM AND building requirements are late! "+str(aBuildingTechLocList)+" "+str(iTechLoc))

				#Analyze GOM OR Building reqs
				aBuildingTechLocList = []
				for iBuildingRequirement in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
					aBuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuildingRequirement]))[0])
				if len(aBuildingTechLocList) > 0 and min(aBuildingTechLocList) > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - GOM OR building requirements are late! "+str(aBuildingTechLocList)+" "+str(iTechLoc))

	#Building replacements of requirements
	def checkBuildingRequirementReplacements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechObsLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
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

			#Generate list of buildings, that replace requirements
			aBuildingRequirementReplacementList = []
			aBuildingReplacementList = []
			for i in xrange(len(aBuildingRequirementList)):
				CvBuildingRequirementInfo = GC.getBuildingInfo(aBuildingRequirementList[i])
				for iBuildingReplacement in xrange(CvBuildingRequirementInfo.getNumReplacementBuilding()):
					iReplacementBuilding = CvBuildingRequirementInfo.getReplacementBuilding(iBuildingReplacement)
					aBuildingRequirementReplacementList.append(iReplacementBuilding)
					aBuildingReplacementList.append(CvBuildingRequirementInfo.getType()+" -> "+GC.getBuildingInfo(iReplacementBuilding).getType())

			#Remove buildings from requirement replacement list, if they already exist in requirement list
			aBuildingRequirementReplacementUniqueList = []
			for i in xrange(len(aBuildingRequirementReplacementList)):
				if aBuildingRequirementReplacementList[i] not in aBuildingRequirementList:
					aBuildingRequirementReplacementUniqueList.append(aBuildingRequirementReplacementList[i])

			#Get replacements of base building
			aBuildingBaseReplacementList = []
			for iBuildingReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				iReplacementBuilding = CvBuildingInfo.getReplacementBuilding(iBuildingReplacement)
				aBuildingBaseReplacementList.append(GC.getBuildingInfo(iReplacementBuilding).getType())

			#Check if tech unlock of replacement is earlier than building obsoletion. Replacements listed aren't part of building requirements
			for i in xrange(len(aBuildingRequirementReplacementUniqueList)):
				CvBuildingReplacementInfo = GC.getBuildingInfo(aBuildingRequirementReplacementUniqueList[i])
				iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingReplacementInfo)[0]
				iTechReplacementObsLoc = self.checkBuildingTechObsoletionLocation(CvBuildingReplacementInfo)[0]
				#Log if requirement is replaced before building obsoletes, if replacement of requirement doesn't replace building itself, and if there isn't common replacement of building requirement and base building replacement.
				if iTechObsLoc > iTechLoc and iTechLoc > 0 and CvBuildingInfo.getType()!= CvBuildingReplacementInfo.getType() and CvBuildingReplacementInfo.getType() not in aBuildingBaseReplacementList:
					self.log(CvBuildingInfo.getType()+" requir. replaced by "+CvBuildingReplacementInfo.getType()+" base obsoletion: "+str(iTechObsLoc)+", requirement replacement unlock/obsoletion: "+str(iTechLoc)+"/"+str(iTechReplacementObsLoc)+", replacements involved: "+str(aBuildingReplacementList)+" Base replacements: "+str(aBuildingBaseReplacementList))

	#Building - civic requirements
	def checkBuildingCivicRequirements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]


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

	#Building earliest manufacturer on resource tech reveal
	def checkBuildingBonusManufacturerTech(self):
		#Index of array - Bonus ID, array value at index - producers highest tech requirement location
		aBonusList = [-1]*GC.getNumBonusInfos()
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]

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

	#Building commerce double time - wonders only, and ensure flat change exists
	def checkBuildingCommerceDoubleTime(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.getCommerceChangeDoubleTime(iCommerce) != 0 and not (isWorldWonder(iBuilding) or isNationalWonder(iBuilding) or  CvBuildingInfo.getHolyCity() != -1):
					self.log(CvBuildingInfo.getType()+" Non-wonder has commerce change double time")

			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.getCommerceChangeDoubleTime(iCommerce) != 0 and CvBuildingInfo.getCommerceChange(iCommerce) == 0:
					self.log(CvBuildingInfo.getType()+" has commerce change double time but no relevant flat commerce change")

	#Building owner change - ensure flat commerce change exists
	def checkBuildingCommerceChangeOriginalOwners(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.isCommerceChangeOriginalOwner(iCommerce) and CvBuildingInfo.getCommerceChange(iCommerce) == 0:
					self.log(CvBuildingInfo.getType()+" has CommerceChangeOriginalOwners but no flat commerce change")

	#Building hurry modifiers works only on buildable buildings
	def checkBuildingHurryModifier(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			if CvBuildingInfo.getProductionCost() == -1 and (CvBuildingInfo.getHurryCostModifier() != 0 or CvBuildingInfo.getHurryAngerModifier() != 0):
				self.log(CvBuildingInfo.getType()+" can't be hurried at first place")

	#Buildings shouldn't obsolete too fast in relation of tech unlock
	def checkBuildingUnlockObsoletion(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			if CvBuildingInfo.getObsoleteTech() != -1:
				iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
				if iObsoleteTechLoc - iTechLoc <= 5:
					self.log(CvBuildingInfo.getType()+" Unlock: "+str(iTechLoc)+" Obsoletion: "+str(iObsoleteTechLoc)+" Difference: "+str(iObsoleteTechLoc - iTechLoc))

	#Building replacements shouldn't obsolete too fast for sanity of beeliners, replacements also shouldn't obsolete at earlier point compared to base
	def checkBuildingReplacementObsoletion(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			iObsoleteTechLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]

			#Gather data about building replacements
			aReplacementTypeList = []
			aReplacementTechUnlockList = []
			aReplacementTechObsoleteList = []
			for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				CvBuildingReplacement = GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(iReplacement))
				aReplacementTypeList.append(CvBuildingReplacement.getType())
				aReplacementTechUnlockList.append(self.checkBuildingTechRequirementLocation(CvBuildingReplacement)[0])
				aReplacementTechObsoleteList.append(self.checkBuildingTechObsoletionLocation(CvBuildingReplacement)[0])

			#If there is any replacement
			iMinReplacementUnlock = -1 #Earliest unlocked replacement - may be without tech requirement at first place
			iMaxReplacementUnlock = -1	#Latest unlocked replacement - buildings list all replacements of replacements
			iMinReplacementObsoletion = -1 #Replacement, that obsoletes earliest - may be correlated to earliest replacement, but often some replaced buildings obsolete together
			iMaxReplacementObsoletion = -1 #Replacement, that obsoletes latest - may be not obsoleting altogether
			if len(aReplacementTypeList) > 0:
				for i in xrange(len(aReplacementTypeList)):
					if aReplacementTechUnlockList[i] == min(aReplacementTechUnlockList):
						iMinReplacementUnlock = i
					if aReplacementTechUnlockList[i] == max(aReplacementTechUnlockList):
						iMaxReplacementUnlock = i
					if aReplacementTechObsoleteList[i] == min(aReplacementTechObsoleteList):
						iMinReplacementObsoletion = i
					if aReplacementTechObsoleteList[i] == max(aReplacementTechObsoleteList):
						iMaxReplacementObsoletion = i

				if iObsoleteTechLoc - aReplacementTechObsoleteList[iMinReplacementObsoletion] > 0:
					self.log(CvBuildingInfo.getType()+" -> "+aReplacementTypeList[iMinReplacementObsoletion]+" base obsoletion/replacement obsoletion "+str(iObsoleteTechLoc)+"/"+str(aReplacementTechObsoleteList[iMinReplacementObsoletion]))

	#Building obsoletion of requirements - requirements shouldn't obsolete before building itself
	def checkBuildingRequirementObsoletion(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)

			BuildingObsoleteTech = CvBuildingInfo.getObsoleteTech()
			BuildingObsoleteTechLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]

			#<PrereqInCityBuildings> - require all buildings in list
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuilding)
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0]
				aBuildingRequirementObsoleteTechLocList.append(BuildingReqTechObsLoc)
				aBuildingRequirementNameList.append(BuildingReqName)
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has AND requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))

			#<PrereqOrBuildings> - require one building in list
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuilding)
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0]
				aBuildingRequirementObsoleteTechLocList.append(BuildingReqTechObsLoc)
				aBuildingRequirementNameList.append(BuildingReqName)
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has latest OR requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))

			#<PrereqAmountBuildings> - require all buildings in empire in list
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementNameList = []
			for pair in CvBuildingInfo.getPrereqNumOfBuildings():
				iPrereqBuilding = pair.id
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0]
				aBuildingRequirementObsoleteTechLocList.append(BuildingReqTechObsLoc)
				aBuildingRequirementNameList.append(BuildingReqName)
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has Empire AND requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))

			#<ConstructCondition>
			aBuildingGOMReqList = []
			for i in range(2):
				aBuildingGOMReqList.append([])
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aBuildingGOMReqList)

			#Analyze GOM AND Building reqs
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0]
				aBuildingRequirementObsoleteTechLocList.append(BuildingReqTechObsLoc)
				aBuildingRequirementNameList.append(BuildingReqName)
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and min(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has GOM AND requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))

			#Analyze GOM OR Building reqs
			aBuildingRequirementObsoleteTechLocList = []
			aBuildingRequirementNameList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				iPrereqBuilding = aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = self.checkBuildingTechObsoletionLocation(GC.getBuildingInfo(iPrereqBuilding))[0]
				aBuildingRequirementObsoleteTechLocList.append(BuildingReqTechObsLoc)
				aBuildingRequirementNameList.append(BuildingReqName)
			if len(aBuildingRequirementObsoleteTechLocList) > 0 and max(aBuildingRequirementObsoleteTechLocList) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has GOM OR requirements obsolete before itself "+str(aBuildingRequirementNameList)+str(aBuildingRequirementObsoleteTechLocList)+" "+str(BuildingObsoleteTechLoc))

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

	#Building tech changes and modifiers
	def checkBuildingTechMods(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
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

					#<BonusAidModifiers>
					#if CvBuildingInfo.getBonusAidModifiers(iBonus) != 0:
					#	self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusAidModifiers")

					#<BonusDefenseChanges>
					if CvBuildingInfo.getBonusDefenseChanges(iBonus) != 0:
						self.log(CvBuildingInfo.getType()+" obsoletes before "+CvBonusInfo.getType()+" Tech enable - BonusDefenseChanges")

	#Buildings X -> Y: X shouldn't be obsolete before Y is available, and X should be unlocked before Y is obsolete
	def checkBuildingAffectingBuildings(self):
		aAffectedBuildingTechUnlockList = []
		aAffectedBuildingTechObsoletionList = []
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			aAffectedBuildingTechUnlockList.append(self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0])
			aAffectedBuildingTechObsoletionList.append(self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0])

		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvAffectingBuildingInfo = GC.getBuildingInfo(iBuilding)
			iAffectingBuildingUnlockTechLoc = self.checkBuildingTechRequirementLocation(CvAffectingBuildingInfo)[0]
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

	#Building - Check if we don't have implicit replacements, also ensure that listed ones aren't unlocked before building
	def checkBuildingImplicitReplacements(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			iBaseTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]

			#Replacements
			aReplacementBuildingList = []
			aReplacementBuildingTechLocList = []
			for i in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				iReplacementBuilding = CvBuildingInfo.getReplacementBuilding(i)
				CvBuildingReplacementInfo = GC.getBuildingInfo(iReplacementBuilding)
				aReplacementBuildingList.append(iReplacementBuilding)
				iReplacementTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingReplacementInfo)[0]
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
				aBuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0])
			if len(aBuildingTechLocList) > 0 and max(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" is unlocked before its AND building requirements "+str(aBuildingTechLocList)+" "+str(iTechLoc))

			#<PrereqOrBuildings> - require one building in list
			aBuildingTechLocList = []
			for iBuilding in xrange(CvUnitInfo.getPrereqOrBuildingsNum()):
				aBuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0])
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
				aBuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]))[0])
			if len(aBuildingTechLocList) > 0 and max(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" - GOM AND building requirements are late! "+str(aBuildingTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Building reqs
			aBuildingTechLocList = []
			for iBuilding in xrange(len(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				aBuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(aBuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]))[0])
			if len(aBuildingTechLocList) > 0 and min(aBuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" - GOM OR building requirements are late! "+str(aBuildingTechLocList)+" "+str(iTechLoc))

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
					if GC.getInfoTypeForString("MAPCATEGORY_EARTH") not in CvBuildingInfo.getMapCategories(): # Exclude space based
						if CvBuildingInfo.getFreeBonus() == iBonus:
							aNumBonusManufacturers.append(self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0])
							aBuildingObsoletions.append(self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0])

						for iBonuses in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
							if CvBuildingInfo.getExtraFreeBonus(iBonuses) == iBonus:
								aNumBonusManufacturers.append(self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0])
								aBuildingObsoletions.append(self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0])

				# Check all bonus producers, that don't obsolete
				if len(aNumBonusManufacturers) > 0 and min(aBuildingObsoletions) == 999:
					for iBuilding in xrange(GC.getNumBuildingInfos()):
						CvBuildingInfo = GC.getBuildingInfo(iBuilding)
						if GC.getInfoTypeForString("MAPCATEGORY_EARTH") not in CvBuildingInfo.getMapCategories(): # Exclude space based
							aBuildingReplacements = [] # List building replacements
							iObsoleteTechLoc = self.checkBuildingTechObsoletionLocation(CvBuildingInfo)[0]
							for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
								CvBuildingReplacement = GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(iReplacement))
								aBuildingReplacements.append(CvBuildingReplacement.getType())

							if CvBuildingInfo.getFreeBonus() == iBonus:
								self.log(CvBonusInfo.getType()+" "+str(self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0])+"/"+str(iObsoleteTechLoc)+" Type: "+CvBuildingInfo.getType()+" Replacement: "+str(aBuildingReplacements))

							for iBonuses in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
								if CvBuildingInfo.getExtraFreeBonus(iBonuses) == iBonus:
									self.log(CvBonusInfo.getType()+" "+str(self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0])+"/"+str(iObsoleteTechLoc)+" Type: "+CvBuildingInfo.getType()+" Replacement: "+str(aBuildingReplacements))

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

	#Civic - list civics and civic categories
	def listCivics(self):
		for iCivic in xrange(GC.getNumCivicInfos()):
			CvCivicInfo = GC.getCivicInfo(iCivic)
			TechReq = CvCivicInfo.getTechPrereq()
			iTechLoc = self.checkCivicTechRequirementLocation(CvCivicInfo)[0]

			if TechReq == -1:
				TechReq = "None"
			else:
				TechReq = GC.getTechInfo(TechReq).getType()
			self.log(GC.getCivicOptionInfo(CvCivicInfo.getCivicOptionType()).getType()+" "+CvCivicInfo.getType()+" "+TechReq+" "+str(iTechLoc))

	def listSpaceRequiringEarthBuildings(self):
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			#Earthly building must have map category of Earth
			if GC.getInfoTypeForString("MAPCATEGORY_EARTH") not in CvBuildingInfo.getMapCategories():
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

				#Analyze requirements
				for i in xrange(len(aBuildingRequirementList)):
					CvRequiredBuildingInfo = GC.getBuildingInfo(aBuildingRequirementList[i])
					if GC.getInfoTypeForString("MAPCATEGORY_EARTH") not in CvBuildingInfo.getMapCategories():
						self.log(CvBuildingInfo.getType()+" requires off-earth building: "+CvRequiredBuildingInfo.getType())