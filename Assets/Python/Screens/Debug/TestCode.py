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
		self.main.addTestCode(screen, self.checkBuildingCivicRequirements, "Building - Civic requirements", "Checks if various civics aren't unlocked after building")
		self.main.addTestCode(screen, self.checkBuildingRequirementObsoletion, "Building obsoletion of requirements", "Checks when requirements obsolete in relation to building itself")
		self.main.addTestCode(screen, self.checkBuildingTechMods, "Building tech changes and modifiers", "Checks if tech modifiers and changes occur within building lifetime")
		self.main.addTestCode(screen, self.checkBuildingBonusManufacturerTech, "Building earliest manufacturer on resource tech reveal", "Checks when earliest resource producer is unlocked")
		self.main.addTestCode(screen, self.checkBuildingCommerceDoubleTime, "Building commerce double time", "Checks if commerce double time exists on wonders, that have relevant flat commerce change")
		self.main.addTestCode(screen, self.checkBuildingCommerceChangeOriginalOwners, "Building commerce owner change", "Checks if Commerce Change has relevant flat commerce changes")
		self.main.addTestCode(screen, self.checkBuildingUnlockObsoletion, "Buildings unlock/obsoletion", "Checks if building obsoletion doesn't happen within 5 columns of building unlock")
		self.main.addTestCode(screen, self.checkBuildingReplacementObsoletion, "Building replacements obsoletion", "Checks when replacements are unlocked and obsoleted")
		self.main.addTestCode(screen, self.checkBuildingWonderMovies, "Building movie wonder list", "Checks movies of noncultural wonders, religious shrines and projects movie location")
		self.main.addTestCode(screen, self.checkBuildingReligionRequirement, "Building religion requirement test", "Checks if tags requiring religion share same religion")
		self.main.addTestCode(screen, self.checkUnitUpgrades, "Unit - check unit upgrades", "Checks unit upgrades")
		self.main.addTestCode(screen, self.checkUnitBonusRequirements, "Unit - check bonus requirements", "Checks bonus requirements of units")
		self.main.addTestCode(screen, self.checkUnitRequirements, "Unit - check building requirements", "Checks building requirements of units")
		self.main.addTestCode(screen, self.checkBonusImprovementProductivity, "Bonus - check improvement productivity", "Checks if improvement replacements productivity from bonus, improvement and bonus+improvement is higher compared to base improvement")
		self.main.addTestCode(screen, self.checkBonusProducerReplacements, "Bonus - check potential bonus producer replacements", "Checks replacements of manufactured bonus producers")
		self.main.addTestCode(screen, self.checkImprovementTechYieldBoostLocation, "Improvement - yield boost tech requirements", "Checks if yield boosts happen within tech unlock and replacement of improvements")
		self.main.addTestCode(screen, self.checkImprovementYieldValues, "Improvement - all techs boosts compared to upgrade", "Checks if improvement with all tech boosts isn't better than its upgrade")
		self.main.addTestCode(screen, self.listCivics, "Civic - list civics and civic categories", "Creates list civics and civic options")

	##### HIGHEST TECH REQUIREMENT LOCATION FINDER FUNCTIONS  #####

	#Building tech location
	def checkBuildingTechRequirementLocation(self, CvBuildingInfo):
		#Main tech requirement
		TechMainReq = CvBuildingInfo.getPrereqAndTech()
		if TechMainReq != -1:
			iTechMainLoc = GC.getTechInfo(TechMainReq).getGridX()
			iTechMainRow = GC.getTechInfo(TechMainReq).getGridY()
		else:
			iTechMainLoc = 0
			iTechMainRow = 0

		#Tech Type requirement
		TechTypeLocList = []
		TechTypeRowList = []
		for techType in CvBuildingInfo.getPrereqAndTechs():
			TechTypeLocList.append(GC.getTechInfo(techType).getGridX())
			TechTypeRowList.append(GC.getTechInfo(techType).getGridY())
		if len(TechTypeLocList) > 0 and len(TechTypeRowList) > 0:
			iTechTypeLoc = max(TechTypeLocList)
			for t in xrange(len(TechTypeLocList)):
				if TechTypeLocList[t] == max(TechTypeLocList):
					iTechTypeRow = TechTypeRowList[t]
		else:
			iTechTypeLoc = 0
			iTechTypeRow = 0

		#Tech requirement as defined in special building infos (core tech)
		iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()
		if iSpecialBuilding != -1:
			TechSpecialReq = GC.getSpecialBuildingInfo(iSpecialBuilding).getTechPrereq()
			if TechSpecialReq != -1:
				iTechSpecialLoc = GC.getTechInfo(TechSpecialReq).getGridX()
				iTechSpecialRow = GC.getTechInfo(TechSpecialReq).getGridY()
			elif TechSpecialReq == -1:
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
				TechReligionReq = GC.getReligionInfo(iReligionBuilding).getTechPrereq()
				if TechReligionReq != -1:
					iTechReligionLoc = GC.getTechInfo(TechReligionReq).getGridX()
					iTechReligionRow = GC.getTechInfo(TechReligionReq).getGridY()
			elif iReligionBuilding == -1:
				iTechReligionLoc = 0
				iTechReligionRow = 0
		else:
			iTechReligionLoc = 0
			iTechReligionRow = 0

		#Tech GOM requirements
		TechGOMReqList = []
		iTechGOMAndLocList = []
		iTechGOMAndRowList = []
		iTechGOMOrLocList = []
		iTechGOMOrRowList = []
		for i in range(2):
			TechGOMReqList.append([])
		self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_TECH, TechGOMReqList)

		#Extract GOM AND requirements
		for i in xrange(len(TechGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
			iTechGOMAndLocList.append(GC.getTechInfo(TechGOMReqList[BoolExprTypes.BOOLEXPR_AND][i]).getGridX())
			iTechGOMAndRowList.append(GC.getTechInfo(TechGOMReqList[BoolExprTypes.BOOLEXPR_AND][i]).getGridY())
		if len(iTechGOMAndLocList) > 0 and len(iTechGOMAndRowList) > 0:
			iTechGOMAndLoc = max(iTechGOMAndLocList)
			for t in xrange(len(iTechGOMAndLocList)):
				if iTechGOMAndLocList[t] == max(iTechGOMAndLocList):
					iTechGOMAndRow = iTechGOMAndRowList[t]
		else:
			iTechGOMAndLoc = 0
			iTechGOMAndRow = 0

		#Extract GOM OR requirements
		for i in xrange(len(TechGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
			iTechGOMOrLocList.append(GC.getTechInfo(TechGOMReqList[BoolExprTypes.BOOLEXPR_OR][i]).getGridX())
			iTechGOMOrRowList.append(GC.getTechInfo(TechGOMReqList[BoolExprTypes.BOOLEXPR_OR][i]).getGridY())
		if len(iTechGOMOrLocList) > 0 and len(iTechGOMOrRowList) > 0:
			iTechGOMOrLoc = max(iTechGOMOrLocList)
			for t in xrange(len(iTechGOMOrLocList)):
				if iTechGOMOrLocList[t] == max(iTechGOMOrLocList):
					iTechGOMOrRow = iTechGOMOrRowList[t]
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
		TechMainReq = CvUnitInfo.getPrereqAndTech()
		if TechMainReq != -1:
			iTechMainLoc = GC.getTechInfo(TechMainReq).getGridX()
			iTechMainRow = GC.getTechInfo(TechMainReq).getGridY()
		else:
			iTechMainLoc = 0
			iTechMainRow = 0

		#Tech Type requirement
		TechTypeLocList = []
		TechTypeRowList = []
		for techType in CvUnitInfo.getPrereqAndTechs():
			TechTypeLocList.append(GC.getTechInfo(techType).getGridX())
			TechTypeRowList.append(GC.getTechInfo(techType).getGridY())
		if len(TechTypeLocList) > 0 and len(TechTypeRowList) > 0:
			iTechTypeLoc = max(TechTypeLocList)
			for t in xrange(len(TechTypeLocList)):
				if TechTypeLocList[t] == max(TechTypeLocList):
					iTechTypeRow = TechTypeRowList[t]
		else:
			iTechTypeLoc = 0
			iTechTypeRow = 0

		#Tech GOM requirements
		TechGOMReqList = []
		iTechGOMAndLocList = []
		iTechGOMAndRowList = []
		iTechGOMOrLocList = []
		iTechGOMOrRowList = []
		for i in range(2):
			TechGOMReqList.append([])
		self.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_TECH, TechGOMReqList)

		#Extract GOM AND requirements
		for i in xrange(len(TechGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
			iTechGOMAndLocList.append(GC.getTechInfo(TechGOMReqList[BoolExprTypes.BOOLEXPR_AND][i]).getGridX())
			iTechGOMAndRowList.append(GC.getTechInfo(TechGOMReqList[BoolExprTypes.BOOLEXPR_AND][i]).getGridY())
		if len(iTechGOMAndLocList) > 0 and len(iTechGOMAndRowList) > 0:
			iTechGOMAndLoc = max(iTechGOMAndLocList)
			for t in xrange(len(iTechGOMAndLocList)):
				if iTechGOMAndLocList[t] == max(iTechGOMAndLocList):
					iTechGOMAndRow = iTechGOMAndRowList[t]
		else:
			iTechGOMAndLoc = 0
			iTechGOMAndRow = 0

		#Extract GOM OR requirements
		for i in xrange(len(TechGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
			iTechGOMOrLocList.append(GC.getTechInfo(TechGOMReqList[BoolExprTypes.BOOLEXPR_OR][i]).getGridX())
			iTechGOMOrRowList.append(GC.getTechInfo(TechGOMReqList[BoolExprTypes.BOOLEXPR_OR][i]).getGridY())
		if len(iTechGOMOrLocList) > 0 and len(iTechGOMOrRowList) > 0:
			iTechGOMOrLoc = max(iTechGOMOrLocList)
			for t in xrange(len(iTechGOMOrLocList)):
				if iTechGOMOrLocList[t] == max(iTechGOMOrLocList):
					iTechGOMOrRow = iTechGOMOrRowList[t]
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
	#GOMReqList = []
	#for i in range(2):
	#	GOMReqList.append([])
	#getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BONUS, GOMReqList) - for buildings
	#getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BONUS, GOMReqList) - for units
	#Array is filled with enums - GC.getBonusInfo(GOMReqList[BoolExprTypes.BOOLEXPR_AND][i]).getType() will extract type of bonus at i-th place.
	#Array is filled with enums - GC.getBonusInfo(GOMReqList[BoolExprTypes.BOOLEXPR_OR][i]).getType() will extract type of bonus at i-th place.

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

	#Building tech requirement list
	def listBuildingTechRequirements(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			techList = []
			#Main tech requirement
			TechMainReq = CvBuildingInfo.getPrereqAndTech()
			if TechMainReq != -1:
				TechMain = GC.getTechInfo(TechMainReq).getType()
				techList.append(TechMain)

			#Tech Type requirement
			for techType in CvBuildingInfo.getPrereqAndTechs():
				techList.append(GC.getTechInfo(techType).getType())

			#Tech requirement as defined in special building infos (core tech)
			iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()
			if iSpecialBuilding != -1:
				TechSpecialReq = GC.getSpecialBuildingInfo(iSpecialBuilding).getTechPrereq()
				if TechSpecialReq != -1:
					TechSpecial = GC.getTechInfo(TechSpecialReq).getType()
					techList.append(TechSpecial)

			#tech requirement derived from location of religion in tech tree
			iRelPrereq1 = CvBuildingInfo.getPrereqReligion()
			iRelPrereq2 = CvBuildingInfo.getReligionType()
			iRelPrereq3 = CvBuildingInfo.getPrereqStateReligion()
			if iRelPrereq1 != -1 or iRelPrereq2 != -1 or iRelPrereq3 != -1:
				iReligionBuilding = max(iRelPrereq1, iRelPrereq2, iRelPrereq3)
				if iReligionBuilding != -1:
					TechReligionReq = GC.getReligionInfo(iReligionBuilding).getTechPrereq()
					if TechReligionReq != -1:
						TechReligionLoc = GC.getTechInfo(TechReligionReq).getType()
						techList.append(TechReligionLoc)

			if len(techList) > 0:
				self.log(CvBuildingInfo.getType()+" requires: "+str(techList))

	#Unit tech requirement list
	def listUnitTechRequirements(self):
		for i in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(i)
			techList = []
			#Main tech
			TechMainReq = CvUnitInfo.getPrereqAndTech()
			if TechMainReq != -1:
				TechMain = GC.getTechInfo(TechMainReq).getType()
				techList.append(TechMain)

			#Tech Type requirement
			for techType in CvUnitInfo.getPrereqAndTechs():
				techList.append(GC.getTechInfo(techType).getType())

			if len(techList) > 0:
				self.log(CvUnitInfo.getType()+" requires: "+str(techList))

	#Tech requirement list
	def listTechRequirements(self):
		for i in xrange(GC.getNumTechInfos()):
			CvTechInfo = GC.getTechInfo(i)
			techList = []
			#AND techs
			for techType in CvTechInfo.getPrereqAndTechs():
				techList.append(GC.getTechInfo(techType).getType())

			if len(CvTechInfo.getPrereqAndTechs()) == 0 and len(CvTechInfo.getPrereqOrTechs()) > 1:
				techList.append("one of: ")
			if len(CvTechInfo.getPrereqAndTechs()) > 0 and len(CvTechInfo.getPrereqOrTechs()) > 1:
				techList.append(" and one of: ")

			#OR techs
			for techType in CvTechInfo.getPrereqOrTechs():
				techList.append(GC.getTechInfo(techType).getType())

			if len(techList) > 0:
				self.log(CvTechInfo.getType()+" requires: "+str(techList))

	#Building bonus requirements
	def checkBuildingBonusRequirements(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			#<Bonus>BONUS_X
			iBuildingBonusReq = CvBuildingInfo.getPrereqAndBonus()
			if iBuildingBonusReq != -1:
				bonusTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBuildingBonusReq))[2]
				if bonusTechLoc > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - Singular AND bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<VicinityBonus>BONUS_X
			iBuildingVicinityBonusReq = CvBuildingInfo.getPrereqVicinityBonus()
			if iBuildingVicinityBonusReq != -1:
				bonusTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBuildingVicinityBonusReq))[2]
				if bonusTechLoc > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - Singular AND vicinity bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<RawVicinityBonus>BONUS_X
			iBuildingRawVicinityBonusReq = CvBuildingInfo.getPrereqRawVicinityBonus()
			if iBuildingRawVicinityBonusReq != -1:
				bonusTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(iBuildingRawVicinityBonusReq))[0]
				if bonusTechLoc > iTechLoc:
					self.log(CvBuildingInfo.getType()+" - Singular AND raw vicinity bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<PrereqBonuses>
			bonusTechLocList = []
			for bonusOr in CvBuildingInfo.getPrereqOrBonuses():
				bonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(bonusOr))[2])
			if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - Earliest OR bonus prereq late! "+str(bonusTechLocList)+" "+str(iTechLoc))

			#<PrereqVicinityBonuses>
			bonusTechLocList = []
			for bonusOrVic in CvBuildingInfo.getPrereqOrVicinityBonuses():
				bonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(bonusOrVic))[2])
			if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - Earliest OR vicinity bonus prereq late! "+str(bonusTechLocList)+" "+str(iTechLoc))

			#<PrereqRawVicinityBonuses>
			bonusTechLocList = []
			for bonusOrVicRaw in CvBuildingInfo.getPrereqOrRawVicinityBonuses():
				bonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(bonusOrVicRaw))[0])
			if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - Earliest OR raw vicinity bonus prereq late! "+str(bonusTechLocList)+" "+str(iTechLoc))

			#<ConstructCondition>
			BonusGOMReqList = []
			for i in range(2):
				BonusGOMReqList.append([])
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BONUS, BonusGOMReqList)

			#Analyze GOM AND Bonus reqs
			bonusTechLocList = []
			for bonusGOMAnd in xrange(len(BonusGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				bonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(BonusGOMReqList[BoolExprTypes.BOOLEXPR_AND][bonusGOMAnd]))[2])
			if len(bonusTechLocList) > 0 and max(bonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - GOM AND bonus requirements are late! "+str(bonusTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Bonus reqs
			bonusTechLocList = []
			for bonusGOMOr in xrange(len(BonusGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				bonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(BonusGOMReqList[BoolExprTypes.BOOLEXPR_OR][bonusGOMOr]))[2])
			if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
				self.log(CvBuildingInfo.getType()+" - GOM OR bonus requirements are late! "+str(bonusTechLocList)+" "+str(iTechLoc))

	#Building requirements of buildings
	def checkBuildingRequirements(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			#<PrereqInCityBuildings> - require all buildings in list
			BuildingTechLocList = []
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuilding)
				BuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
			if len(BuildingTechLocList) > 0 and max(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" is unlocked before its AND building requirements "+str(BuildingTechLocList)+" "+str(iTechLoc))

			#<PrereqOrBuildings> - require one building in list
			BuildingTechLocList = []
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuilding)
				BuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
			if len(BuildingTechLocList) > 0 and min(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" is unlocked before its earliest OR building requirement "+str(BuildingTechLocList)+" "+str(iTechLoc))

			#<PrereqAmountBuildings> - require all buildings in empire in list
			BuildingTechLocList = []
			for pair in CvBuildingInfo.getPrereqNumOfBuildings():
				iPrereqBuilding = pair.id
				BuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iPrereqBuilding))[0])
			if len(BuildingTechLocList) > 0 and max(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" is unlocked before its Empire AND requirement "+str(BuildingTechLocList)+" "+str(iTechLoc))
				
			#<ConstructCondition>
			BuildingGOMReqList = []
			for i in range(2):
				BuildingGOMReqList.append([])
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, BuildingGOMReqList)

			#Analyze GOM AND Building reqs
			BuildingTechLocList = []
			for iBuilding in xrange(len(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				BuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]))[0])
			if len(BuildingTechLocList) > 0 and max(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" - GOM AND building requirements are late! "+str(BuildingTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Building reqs
			BuildingTechLocList = []
			for iBuilding in xrange(len(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				BuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]))[0])
			if len(BuildingTechLocList) > 0 and min(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" - GOM OR building requirements are late! "+str(BuildingTechLocList)+" "+str(iTechLoc))

	#Building - Civic requirements
	def checkBuildingCivicRequirements(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]

			#<PrereqAndCivics> - require all civics in list
			BuildingAndCivicTechLocList = []
			for iCivic in xrange(GC.getNumCivicInfos()):
				if CvBuildingInfo.isPrereqAndCivics(iCivic):
					BuildingAndCivicTechLoc = self.checkCivicTechRequirementLocation(GC.getCivicInfo(iCivic))[0]
					BuildingAndCivicTechLocList.append(BuildingAndCivicTechLoc)
			if len(BuildingAndCivicTechLocList) > 0 and max(BuildingAndCivicTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" is unlocked before its civic AND requirements "+str(BuildingAndCivicTechLocList)+" "+str(iTechLoc))

			#<PrereqOrCivics> - require one civics in list
			BuildingOrCivicTechLocList = []
			for iCivic in xrange(GC.getNumCivicInfos()):
				if CvBuildingInfo.isPrereqOrCivics(iCivic):
					BuildingOrCivicTechLoc = self.checkCivicTechRequirementLocation(GC.getCivicInfo(iCivic))[0]
					BuildingOrCivicTechLocList.append(BuildingOrCivicTechLoc)
			if len(BuildingOrCivicTechLocList) > 0 and min(BuildingOrCivicTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvBuildingInfo.getType()+" is unlocked before its earliest OR civic requirement "+str(BuildingOrCivicTechLocList)+" "+str(iTechLoc))

	#Building obsoletion of requirements
	def checkBuildingRequirementObsoletion(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)

			BuildingObsoleteTech = CvBuildingInfo.getObsoleteTech()
			BuildingObsoleteTechLoc = 999
			if BuildingObsoleteTech != -1:
				BuildingObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()

			#<PrereqInCityBuildings> - require all buildings in list
			BuildingRequirementObsoleteTechLoc = []
			BuildingRequirementName = []
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqInCityBuildings()):
				iPrereqBuilding = CvBuildingInfo.getPrereqInCityBuilding(iBuilding)
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = 999
				if BuildingReqTechObs != -1:
					BuildingReqTechObsLoc = GC.getTechInfo(BuildingReqTechObs).getGridX()
				BuildingRequirementObsoleteTechLoc.append(BuildingReqTechObsLoc)
				BuildingRequirementName.append(BuildingReqName)
			if len(BuildingRequirementObsoleteTechLoc) > 0 and min(BuildingRequirementObsoleteTechLoc) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has AND requirements obsolete before itself "+str(BuildingRequirementName)+str(BuildingRequirementObsoleteTechLoc)+" "+str(BuildingObsoleteTechLoc))

			#<PrereqOrBuildings> - require one building in list
			BuildingRequirementObsoleteTechLoc = []
			BuildingRequirementName = []
			for iBuilding in xrange(CvBuildingInfo.getNumPrereqOrBuilding()):
				iPrereqBuilding = CvBuildingInfo.getPrereqOrBuilding(iBuilding)
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = 999
				if BuildingReqTechObs != -1:
					BuildingReqTechObsLoc = GC.getTechInfo(BuildingReqTechObs).getGridX()
				BuildingRequirementObsoleteTechLoc.append(BuildingReqTechObsLoc)
				BuildingRequirementName.append(BuildingReqName)
			if len(BuildingRequirementObsoleteTechLoc) > 0 and max(BuildingRequirementObsoleteTechLoc) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has latest OR requirements obsolete before itself "+str(BuildingRequirementName)+str(BuildingRequirementObsoleteTechLoc)+" "+str(BuildingObsoleteTechLoc))

			#<PrereqAmountBuildings> - require all buildings in empire in list
			BuildingRequirementObsoleteTechLoc = []
			BuildingRequirementName = []
			for pair in CvBuildingInfo.getPrereqNumOfBuildings():
				iPrereqBuilding = pair.id
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = 999
				if BuildingReqTechObs != -1:
					BuildingReqTechObsLoc = GC.getTechInfo(BuildingReqTechObs).getGridX()
				BuildingRequirementObsoleteTechLoc.append(BuildingReqTechObsLoc)
				BuildingRequirementName.append(BuildingReqName)
			if len(BuildingRequirementObsoleteTechLoc) > 0 and min(BuildingRequirementObsoleteTechLoc) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has Empire AND requirements obsolete before itself "+str(BuildingRequirementName)+str(BuildingRequirementObsoleteTechLoc)+" "+str(BuildingObsoleteTechLoc))
				
			#<ConstructCondition>
			BuildingGOMReqList = []
			for i in range(2):
				BuildingGOMReqList.append([])
			self.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, BuildingGOMReqList)
			
			#Analyze GOM AND Building reqs
			BuildingRequirementObsoleteTechLoc = []
			BuildingRequirementName = []
			for iBuilding in xrange(len(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				iPrereqBuilding = BuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = 999
				if BuildingReqTechObs != -1:
					BuildingReqTechObsLoc = GC.getTechInfo(BuildingReqTechObs).getGridX()
				BuildingRequirementObsoleteTechLoc.append(BuildingReqTechObsLoc)
				BuildingRequirementName.append(BuildingReqName)
			if len(BuildingRequirementObsoleteTechLoc) > 0 and min(BuildingRequirementObsoleteTechLoc) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has GOM AND requirements obsolete before itself "+str(BuildingRequirementName)+str(BuildingRequirementObsoleteTechLoc)+" "+str(BuildingObsoleteTechLoc))
				
			#Analyze GOM OR Building reqs
			BuildingRequirementObsoleteTechLoc = []
			BuildingRequirementName = []
			for iBuilding in xrange(len(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				iPrereqBuilding = BuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]
				BuildingReqTechObs = GC.getBuildingInfo(iPrereqBuilding).getObsoleteTech()
				BuildingReqName = GC.getBuildingInfo(iPrereqBuilding).getType()
				BuildingReqTechObsLoc = 999
				if BuildingReqTechObs != -1:
					BuildingReqTechObsLoc = GC.getTechInfo(BuildingReqTechObs).getGridX()
				BuildingRequirementObsoleteTechLoc.append(BuildingReqTechObsLoc)
				BuildingRequirementName.append(BuildingReqName)
			if len(BuildingRequirementObsoleteTechLoc) > 0 and min(BuildingRequirementObsoleteTechLoc) < BuildingObsoleteTechLoc:
				self.log(CvBuildingInfo.getType()+" has GOM OR requirements obsolete before itself "+str(BuildingRequirementName)+str(BuildingRequirementObsoleteTechLoc)+" "+str(BuildingObsoleteTechLoc))

	#Building tech changes and modifiers
	def checkBuildingTechMods(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
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
				iTechMod = 0
				while iTechMod < GC.getNumTechInfos():
					i = 0
					while i < YieldTypes.NUM_YIELD_TYPES:
						if CvBuildingInfo.getTechYieldChange(iTechMod, i):
							iTechMLoc = GC.getTechInfo(iTechMod).getGridX()
							if iTechMLoc <= iTechLoc:
								self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Yield Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Yield Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
						i += 1
					iTechMod += 1

			#Check if Yield Modifiers techs don't appear before building can be unlocked or after is obsoleted
			if CvBuildingInfo.isAnyTechYieldModifiers():
				iTechMod = 0
				while iTechMod < GC.getNumTechInfos():
					i = 0
					while i < YieldTypes.NUM_YIELD_TYPES:
						if CvBuildingInfo.getTechYieldModifier(iTechMod, i):
							iTechMLoc = GC.getTechInfo(iTechMod).getGridX()
							if iTechMLoc <= iTechLoc:
								self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Yield Modifiers early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Yield Modifiers late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
						i += 1
					iTechMod += 1

			#Check if Commerce Changes techs don't appear before building can be unlocked or after is obsoleted
			if CvBuildingInfo.isAnyTechCommerceChanges():
				iTechMod = 0
				while iTechMod < GC.getNumTechInfos():
					i = 0
					while i < CommerceTypes.NUM_COMMERCE_TYPES:
						if CvBuildingInfo.getTechCommerceChange(iTechMod, i):
							iTechMLoc = GC.getTechInfo(iTechMod).getGridX()
							if iTechMLoc <= iTechLoc:
								self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Commerce Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Commerce Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
						i += 1
					iTechMod += 1

			#Check if Commerce Modifiers techs don't appear before building can be unlocked or after is obsoleted
			if CvBuildingInfo.isAnyTechCommerceModifiers():
				iTechMod = 0
				while iTechMod < GC.getNumTechInfos():
					i = 0
					while i < CommerceTypes.NUM_COMMERCE_TYPES:
						if CvBuildingInfo.getTechCommerceModifier(iTechMod, i):
							iTechMLoc = GC.getTechInfo(iTechMod).getGridX()
							if iTechMLoc <= iTechLoc:
								self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Commerce Modifiers early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Commerce Modifiers late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
						i += 1
					iTechMod += 1

			#Check if Specialist Changes techs don't appear before building can be unlocked or after is obsoleted
			if CvBuildingInfo.isAnyTechSpecialistChanges():
				iTechMod = 0
				while iTechMod < GC.getNumTechInfos():
					i = 0
					while i < GC.getNumSpecialistInfos():
						if CvBuildingInfo.getTechSpecialistChange(iTechMod, i):
							iTechMLoc = GC.getTechInfo(iTechMod).getGridX()
							if iTechMLoc <= iTechLoc:
								self.log(CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Specialist Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								self.log(CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Specialist Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType())
						i += 1
					iTechMod += 1

	#Building earliest manufacturer on resource tech reveal
	def checkBuildingBonusManufacturerTech(self):
		#Index of array - Bonus ID, array value at index - producers highest tech requirement location
		bonuslist = [-1]*GC.getNumBonusInfos()
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]

			#Singular <FreeBonus>
			bonus = CvBuildingInfo.getFreeBonus()
			if bonus != -1:
				if bonuslist[bonus] == -1:
					bonuslist[bonus] = iTechLoc
				elif bonuslist[bonus] != -1 and bonuslist[bonus] > iTechLoc:
					bonuslist[bonus] = iTechLoc

			#<ExtraFreeBonuses>
			for j in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
				bonus = CvBuildingInfo.getExtraFreeBonus(j)
				if bonuslist[bonus] == -1:
					bonuslist[bonus] = iTechLoc
				elif bonuslist[bonus] != -1 and bonuslist[bonus] > iTechLoc:
					bonuslist[bonus] = iTechLoc

		for bonustype in xrange(len(bonuslist)):
			bonustypeTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(bonustype))[2]
			if bonuslist[bonustype] != -1 and GC.getBonusInfo(bonustype).getTechCityTrade() != -1 and not GC.getBonusInfo(bonustype).getConstAppearance() > 0:
				if bonuslist[bonustype] - bonustypeTechLoc != 0:
					self.log(GC.getBonusInfo(bonustype).getType()+" "+str(bonustypeTechLoc)+" Earliest bonus producer located at: "+str(bonuslist[bonustype]))

	#Building commerce double time - wonders only, and ensure flat change exists
	def checkBuildingCommerceDoubleTime(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.getCommerceChangeDoubleTime(iCommerce) != 0 and not (isWorldWonder(i) or isNationalWonder(i) or  CvBuildingInfo.getHolyCity() != -1):
					self.log(CvBuildingInfo.getType()+" Non-wonder has commerce change double time")

			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.getCommerceChangeDoubleTime(iCommerce) != 0 and CvBuildingInfo.getCommerceChange(iCommerce) == 0:
					self.log(CvBuildingInfo.getType()+" has commerce change double time but no relevant flat commerce change")

	#Building owner change - ensure flat commerce change exists
	def checkBuildingCommerceChangeOriginalOwners(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.isCommerceChangeOriginalOwner(iCommerce) and CvBuildingInfo.getCommerceChange(iCommerce) == 0:
					self.log(CvBuildingInfo.getType()+" has CommerceChangeOriginalOwners but no flat commerce change")

	#Building hurry modifiers works only on buildable buildings
	def checkBuildingHurryModifier(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			if CvBuildingInfo.getProductionCost() == -1 and (CvBuildingInfo.getHurryCostModifier() != 0 or CvBuildingInfo.getHurryAngerModifier() != 0):
				self.log(CvBuildingInfo.getType()+" can't be hurried at first place")

	#Buildings shouldn't obsolete too fast in relation of tech unlock
	def checkBuildingUnlockObsoletion(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			if CvBuildingInfo.getObsoleteTech() != -1:
				iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
				if iObsoleteTechLoc - iTechLoc <= 5:
					self.log(CvBuildingInfo.getType()+" Unlock: "+str(iTechLoc)+" Obsoletion: "+str(iObsoleteTechLoc)+" Difference: "+str(iObsoleteTechLoc - iTechLoc))

	#Building replacements shouldn't obsolete too fast for sanity of beeliners
	def checkBuildingReplacementObsoletion(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			if CvBuildingInfo.getObsoleteTech() != -1:
				iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
			else:
				iObsoleteTechLoc = 999
			for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				CvBuildingReplacement = GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(iReplacement))
				iReplacTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingReplacement)[0]
				if CvBuildingReplacement.getObsoleteTech() != -1:
					iReplacObsoleteTechLoc = GC.getTechInfo(CvBuildingReplacement.getObsoleteTech()).getGridX()
				else:
					iReplacObsoleteTechLoc = 999

				if iObsoleteTechLoc <= iReplacTechLoc:
					self.log(CvBuildingInfo.getType()+": "+str(iTechLoc)+"/"+str(iObsoleteTechLoc)+" -> "+CvBuildingReplacement.getType()+": "+str(iReplacTechLoc)+"/"+str(iReplacObsoleteTechLoc))

	#Buildings - noncultural wonders, religious shrines and projects should have wonder movie tag, preferably in DDS format
	def checkBuildingWonderMovies(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()
			bCultural = 0
			if iSpecialBuilding != -1 and iSpecialBuilding == GC.getInfoTypeForString("SPECIALBUILDING_C2C_CULTURE"):
				bCultural = 1

			if isNationalWonder(i) or (isWorldWonder(i) and not bCultural) or CvBuildingInfo.getHolyCity() != -1:
				if CvBuildingInfo.getMovie() != None:
					self.log(CvBuildingInfo.getType()+" has movie in "+CvBuildingInfo.getMovie())
				else:
					self.log(CvBuildingInfo.getType()+" is missing a wonder movie!")

		for i in xrange(GC.getNumProjectInfos()):
			CvProjectInfo = GC.getProjectInfo(i)
			if not CvProjectInfo.isSpaceship():
				if CvProjectInfo.getMovieArtDef() != "":
					self.log(CvProjectInfo.getType()+" has movie art define tag: "+CvProjectInfo.getMovieArtDef())
				else:
					self.log(CvProjectInfo.getType()+" is missing a wonder movie!")

	#Building - ensure that building religious tags always reference same religion
	def checkBuildingReligionRequirement(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iRelPrereq1 = CvBuildingInfo.getPrereqReligion()
			iRelPrereq2 = CvBuildingInfo.getReligionType()
			iRelPrereq3 = CvBuildingInfo.getPrereqStateReligion()

			if iRelPrereq1 != -1 and iRelPrereq2 != -1 and iRelPrereq1 != iRelPrereq2:
				self.log(CvBuildingInfo.getType()+" mismatch between getPrereqReligion and getReligionType: "+GC.getReligionType(iRelPrereq1).getType()+" "+GC.getReligionType(iRelPrereq2).getType())
			if iRelPrereq2 != -1 and iRelPrereq3 != -1 and iRelPrereq2 != iRelPrereq3:
				self.log(CvBuildingInfo.getType()+" mismatch between getReligionType and getPrereqStateReligion: "+GC.getReligionType(iRelPrereq2).getType()+" "+GC.getReligionType(iRelPrereq3).getType())
			if iRelPrereq3 != -1 and iRelPrereq1 != -1 and iRelPrereq3 != iRelPrereq1:
				self.log(CvBuildingInfo.getType()+" mismatch between getPrereqStateReligion and getPrereqReligion: "+GC.getReligionType(iRelPrereq3).getType()+" "+GC.getReligionType(iRelPrereq1).getType())

	#Unit - check unit upgrades
	def checkUnitUpgrades(self):
		for i in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(i)
			iTechLoc = self.checkUnitTechRequirementLocation(CvUnitInfo)[0]

			if CvUnitInfo.getNumUnitUpgrades() > 0:
				iCost = CvUnitInfo.getProductionCost()
				for u in xrange(CvUnitInfo.getNumUnitUpgrades()):
					upgradedDesc = GC.getUnitInfo(CvUnitInfo.getUnitUpgrade(u)).getType()
					upgradedCost = GC.getUnitInfo(CvUnitInfo.getUnitUpgrade(u)).getProductionCost()
					upgradedTechLoc = self.checkUnitTechRequirementLocation(GC.getUnitInfo(CvUnitInfo.getUnitUpgrade(u)))[0]

					dist = upgradedTechLoc - iTechLoc
					costdiff = upgradedCost - iCost

					upgradedUnit = GC.getUnitInfo(CvUnitInfo.getUnitUpgrade(u))
					secondUpgradeList = []
					for u2 in xrange(upgradedUnit.getNumUnitUpgrades()):
						secondUpgradeList.append(GC.getUnitInfo(upgradedUnit.getUnitUpgrade(u2)).getType())
					if CvUnitInfo.getNumUnitUpgrades() == 1:
						self.log(str(iTechLoc)+" - "+str(CvUnitInfo.getType())+"; Upgrade: "+str(upgradedTechLoc)+" - "+str(upgradedDesc)+" -> Distance: "+str(dist)+", Cost difference: "+str(costdiff)+" Upgrade of upgrade "+str(secondUpgradeList))
					elif CvUnitInfo.getNumUnitUpgrades() > 1:
						self.log(str(iTechLoc)+" - "+str(CvUnitInfo.getType())+"; Upgrade #"+str(u+1)+"/"+str(CvUnitInfo.getNumUnitUpgrades())+": "+str(upgradedTechLoc)+" - "+str(upgradedDesc)+" -> Distance: "+str(dist)+", Cost difference: "+str(costdiff)+" Upgrade of upgrade "+str(secondUpgradeList))

	#Unit - check unit bonus requirements
	def checkUnitBonusRequirements(self):
		for i in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(i)
			iTechLoc = self.checkUnitTechRequirementLocation(CvUnitInfo)[0]

			#<BonusType>BONUS_X
			iUnitBonusReq = CvUnitInfo.getPrereqAndBonus()
			if iUnitBonusReq != -1:
				bonusTechLoc = self.checkBonusTechRequirementLocation(GC.getBonusInfo(iUnitBonusReq))[2]
				if bonusTechLoc > iTechLoc:
					self.log(CvUnitInfo.getType()+" - Singular AND bonus prereq late! "+str(bonusTechLoc)+" "+str(iTechLoc))

			#<PrereqBonuses>
			bonusTechLocList = []
			for bonusOr in CvUnitInfo.getPrereqOrBonuses():
				bonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(bonusOr))[2])
			if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
				self.log(CvUnitInfo.getType()+" - Earliest OR bonus prereq late! "+str(bonusTechLocList)+" "+str(iTechLoc))

			#<TrainCondition>
			BonusGOMReqList = []
			for i in range(2):
				BonusGOMReqList.append([])
			self.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BONUS, BonusGOMReqList)

			#Analyze GOM AND Bonus reqs
			bonusTechLocList = []
			for bonusGOMAnd in xrange(len(BonusGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				bonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(BonusGOMReqList[BoolExprTypes.BOOLEXPR_AND][bonusGOMAnd]))[2])
			if len(bonusTechLocList) > 0 and max(bonusTechLocList) > iTechLoc:
				self.log(CvUnitInfo.getType()+" - GOM AND bonus requirements are late! "+str(bonusTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Bonus reqs
			bonusTechLocList = []
			for bonusGOMOr in xrange(len(BonusGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				bonusTechLocList.append(self.checkBonusTechRequirementLocation(GC.getBonusInfo(BonusGOMReqList[BoolExprTypes.BOOLEXPR_OR][bonusGOMOr]))[2])
			if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
				self.log(CvUnitInfo.getType()+" - GOM OR bonus requirements are late! "+str(bonusTechLocList)+" "+str(iTechLoc))

	#Unit - check building requirements
	def checkUnitRequirements(self):
		for i in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(i)
			iTechLoc = self.checkUnitTechRequirementLocation(CvUnitInfo)[0]
			
			#<PrereqAndBuildings> - require all buildings in list
			BuildingTechLocList = []
			for iBuilding in xrange(CvUnitInfo.getNumPrereqAndBuildings()):
				BuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0])
			if len(BuildingTechLocList) > 0 and max(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" is unlocked before its AND building requirements "+str(BuildingTechLocList)+" "+str(iTechLoc))

			#<PrereqOrBuildings> - require one building in list
			BuildingTechLocList = []
			for iBuilding in xrange(CvUnitInfo.getPrereqOrBuildingsNum()):
				BuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0])
			if len(BuildingTechLocList) > 0 and min(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" is unlocked before its earliest OR building requirement "+str(BuildingTechLocList)+" "+str(iTechLoc))

			#<TrainCondition>
			BuildingGOMReqList = []
			for i in range(2):
				BuildingGOMReqList.append([])
			self.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BUILDING, BuildingGOMReqList)

			#Analyze GOM AND Building reqs
			BuildingTechLocList = []
			for iBuilding in xrange(len(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND])):
				BuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_AND][iBuilding]))[0])
			if len(BuildingTechLocList) > 0 and max(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" - GOM AND building requirements are late! "+str(BuildingTechLocList)+" "+str(iTechLoc))

			#Analyze GOM OR Building reqs
			BuildingTechLocList = []
			for iBuilding in xrange(len(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR])):
				BuildingTechLocList.append(self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(BuildingGOMReqList[BoolExprTypes.BOOLEXPR_OR][iBuilding]))[0])
			if len(BuildingTechLocList) > 0 and min(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				self.log(CvUnitInfo.getType()+" - GOM OR building requirements are late! "+str(BuildingTechLocList)+" "+str(iTechLoc))
				
	#Bonus - check improvement productivity
	def checkBonusImprovementProductivity(self):
		for iBonus in xrange(GC.getNumBonusInfos()):
			CvBonusInfo = GC.getBonusInfo(iBonus)
			#Check total productivity: from resource, improvement, improvement+resource coupling, and with all tech boosts.
			if CvBonusInfo.getConstAppearance() > 0: # Only care about map resources
				for i in xrange(GC.getNumImprovementInfos()):
					CvImprovementInfo = GC.getImprovementInfo(i)
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
							for i in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
								CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(i))
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
			# Check Manufactured bonuses, that aren't Culture or Techno-culture types.
				aNumBonusManufacturers = [] # Count manufacturers and add their locations
				aBuildingObsoletions = [] # List xgrid of manufacturer tech obsoletions
				for iBuilding in xrange(GC.getNumBuildingInfos()): # Collect statistics about buildings - location of producer and its obsoletion
					CvBuildingInfo = GC.getBuildingInfo(iBuilding)
					if CvBuildingInfo.isMapType(int(MapTypes.MAP_EARTH)): # Exclude space based
						if CvBuildingInfo.getFreeBonus() == iBonus:
							aNumBonusManufacturers.append(self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0])
							if CvBuildingInfo.getObsoleteTech() != -1:
								iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
								aBuildingObsoletions.append(iObsoleteTechLoc)
							else:
								aBuildingObsoletions.append(999) #Never obsolete

						for iBonuses in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
							if CvBuildingInfo.getExtraFreeBonus(iBonuses) == iBonus:
								aNumBonusManufacturers.append(self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0])
								if CvBuildingInfo.getObsoleteTech() != -1:
									iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
									aBuildingObsoletions.append(iObsoleteTechLoc)
								else:
									aBuildingObsoletions.append(999) #Never obsolete

				# Check all bonus producers, that don't obsolete
				if len(aNumBonusManufacturers) > 0 and min(aBuildingObsoletions) == 999:
					for iBuilding in xrange(GC.getNumBuildingInfos()):
						CvBuildingInfo = GC.getBuildingInfo(iBuilding)
						if CvBuildingInfo.isMapType(int(MapTypes.MAP_EARTH)): # Exclude space based
							aBuildingReplacements = [] # List building replacements
							iObsoleteTechLoc = 999 # Never obsolete
							if CvBuildingInfo.getObsoleteTech() != -1:
								iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
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
				for i in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(i))
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

				for i in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					aBaseAltUpgradeImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
					CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(i))
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