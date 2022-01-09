#Helper Functions - collection of functions meant for further usage
from CvPythonExtensions import *
GC = CyGlobalContext()
aTechID = []
aTechDesc = []

class HelperFunctions:

	def __init__(self, pHelperFunctions):
		self.main = pHelperFunctions

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

		#Folklore handling - X Require tech requirement is treated as one of tech requirements of building, assuming X Require is main building requirement.
		iTechAnimalLoc = 0
		iTechAnimalRow = 0
		for iBuildingRequirement in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
			iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuildingRequirement)
			if iPrereqBuilding == GC.getInfoTypeForString("BUILDING_ANIMAL_FOLKLORE_REQUIRE"):
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
			if aTechGridXList[i] == max(aTechGridXList) and aTechList[i] not in aMostAdvancedColumnRequirementsNames:
				aMostAdvancedColumnRequirementsNames.append(aTechList[i])

		#aMostAdvancedColumnRequirementsXY is a Tech location ID array - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech requirement, then both infotype X/Y grid is 0
		#Otherwise infotype gets highest Xgrid tech requirement and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		aMostAdvancedColumnRequirementsXY = []
		for i in xrange(len(aTechGridXList)):
			if aTechGridXList[i] == max(aTechGridXList) and aTechXY[i] not in aMostAdvancedColumnRequirementsXY:
				aMostAdvancedColumnRequirementsXY.append(aTechXY[i])

		if len(aMostAdvancedColumnRequirementsXY) == 0:
			aMostAdvancedColumnRequirementsXY.append(0)

		return iTechLoc, iTechRow, aMostAdvancedColumnRequirementsXY, aMostAdvancedColumnRequirementsNames

	def checkBuildingEra(self, CvBuildingInfo):
		iEra = 0

		#Main tech requirement
		if CvBuildingInfo.getPrereqAndTech() != -1:
			iEra = GC.getTechInfo(CvBuildingInfo.getPrereqAndTech()).getEra()

		#Tech Type requirement
		for iTech in CvBuildingInfo.getPrereqAndTechs():
			if GC.getTechInfo(iTech).getEra() > iEra:
				iEra = GC.getTechInfo(iTech).getEra()

		#Tech requirement as defined in special building infos (core tech)
		if CvBuildingInfo.getSpecialBuildingType() != -1:
			iTech = GC.getSpecialBuildingInfo(CvBuildingInfo.getSpecialBuildingType()).getTechPrereq()
			if iTech != -1 and GC.getTechInfo(iTech).getEra() > iEra:
				iEra = GC.getTechInfo(iTech).getEra()

		#Tech requirement derived from location of religion in tech tree
		if CvBuildingInfo.getPrereqReligion() != -1:
			iTech = GC.getReligionInfo(CvBuildingInfo.getPrereqReligion()).getTechPrereq()
			if GC.getTechInfo(iTech).getEra() > iEra:
				iEra = GC.getTechInfo(iTech).getEra()
		if CvBuildingInfo.getReligionType() != -1:
			iTech = GC.getReligionInfo(CvBuildingInfo.getReligionType()).getTechPrereq()
			if GC.getTechInfo(iTech).getEra() > iEra:
				iEra = GC.getTechInfo(iTech).getEra()
		if CvBuildingInfo.getPrereqStateReligion() != -1:
			iTech = GC.getReligionInfo(CvBuildingInfo.getPrereqStateReligion()).getTechPrereq()
			if GC.getTechInfo(iTech).getEra() > iEra:
				iEra = GC.getTechInfo(iTech).getEra()

		#Folklore handling - X Require tech requirement is treated as one of tech requirements of building, assuming X Require is main building requirement.
		if CvBuildingInfo.getType().find("BUILDING_FOLKLORE_",0,18) != -1:
			iPrereqBuilding = GC.getInfoTypeForString("BUILDING_ANIMAL_FOLKLORE_REQUIRE")
			if GC.getTechInfo(GC.getBuildingInfo(iPrereqBuilding).getPrereqAndTech()).getEra() > iEra:
				iEra = GC.getTechInfo(GC.getBuildingInfo(iPrereqBuilding).getPrereqAndTech()).getEra()

		#Tech GOM requirements
		aTechGOMReqList = []
		for i in range(2):
			aTechGOMReqList.append([])
		self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_TECH, aTechGOMReqList)

		#Extract GOM AND requirements
		for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
			if GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getEra() > iEra:
				iEra = GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_AND][iTech]).getEra()

		#Extract GOM OR requirements - those are OR type requirements, so pick earliest one.
		aEraList = []
		for iTech in xrange(len(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
			aEraList.append(GC.getTechInfo(aTechGOMReqList[BoolExprTypes.BOOLEXPR_OR][iTech]).getEra())
		if len(aEraList) > 0 and min(aEraList) > iEra:
			iEra = min(aEraList)

		return iEra

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

	##### PROPERTY READER FUNCTIONS #####

	def getPropertyAmmountPerTurn(self, pPropertyManipulators):
		a = [0]*GC.getNumPropertyInfos()
		if pPropertyManipulators is not None:
			for iSource in xrange(pPropertyManipulators.getNumSources()):
				pSource = pPropertyManipulators.getSource(iSource)
				if isinstance(pSource, CvPropertySourceConstant):
					pIntExpr = pSource.getAmountPerTurnExpr()
					if isinstance(pIntExpr, IntExprConstant):
						a[pSource.getProperty()] += pIntExpr.iValue
		return a

	#^^^^ PROPERTY READER FUNCTIONS ^^^^#

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

	#Unit tech obsoletion location
	def checkUnitTechObsoletionLocation(self, CvUnitInfo):
		iTechObsLoc = 999 #Never obsoletes
		iTechObsRow = 99 #Never obsoletes

		if CvUnitInfo.getObsoleteTech() != -1:
			iTechObsLoc = GC.getTechInfo(CvUnitInfo.getObsoleteTech()).getGridX()
			iTechObsRow = GC.getTechInfo(CvUnitInfo.getObsoleteTech()).getGridY()

		#This is a Tech location ID - X grid varies from 0 to 160, and Ygrid varies from 0 to 20
		#If infotype doesn't have tech obsoletion, then infotype X/Y grid is 999 / 99
		#Otherwise infotype gets highest Xgrid tech obsoletion and related Ygrid position
		#Xgrid is multiplied by 100, and then its value is increased by Ygrid
		iTechObsXY = 100*iTechObsLoc + iTechObsRow
		sTechDesc = self.getTechName(iTechObsXY)

		return iTechObsLoc, iTechObsXY, sTechDesc

	#^^^^ OBSOLETION TECH LOCATION FINDER FUNCTIONS ^^^^#
