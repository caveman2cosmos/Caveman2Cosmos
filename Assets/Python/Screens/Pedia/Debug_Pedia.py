# Debug functions 

from CvPythonExtensions import *
GC = CyGlobalContext()

class Debug:

	def __init__(self):
		self.bDebug = True

	#Building tech location
	def checkTechRequirementLocation(self, CvBuildingInfo):
		#Main tech requirement
		TechMainReq = CvBuildingInfo.getPrereqAndTech()
		if GC.getTechInfo(TechMainReq) != None:
			iTechMainLoc = GC.getTechInfo(TechMainReq).getGridX()
			iTechMainRow = GC.getTechInfo(TechMainReq).getGridY()
		else:
			iTechMainLoc = 0
			iTechMainRow = 0

		#Tech Type requirement
		TechTypeLocList = []
		TechTypeRowList = []
		for techType in CvBuildingInfo.getPrereqAndTechs():
			TechTypeReq = techType
			if GC.getTechInfo(TechTypeReq) > -1:
				TechTypeLocList.append(GC.getTechInfo(TechTypeReq).getGridX())
				TechTypeRowList.append(GC.getTechInfo(TechTypeReq).getGridY())
			else:
				TechTypeLocList.append(0)
				TechTypeRowList.append(0)
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

		#tech requirement derived from location of religion in tech tree
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

		#Pick most advanced tech
		iTechLoc = max(iTechMainLoc, iTechTypeLoc, iTechSpecialLoc, iTechReligionLoc)
		if iTechLoc == iTechMainLoc:
			iTechRow = iTechMainRow
		if iTechLoc == iTechTypeLoc:
			iTechRow = iTechTypeRow
		elif iTechLoc == iTechSpecialLoc:
			iTechRow = iTechSpecialRow
		elif iTechLoc == iTechReligionLoc:
			iTechRow = iTechReligionRow

		return iTechLoc, iTechRow
		
	#Building Bonus requirements
	def checkBonusRequirements(self, iTechLoc, CvBuildingInfo):
		#<Bonus>BONUS_X
		iBuildingBonusReq = CvBuildingInfo.getPrereqAndBonus()
		if GC.getBonusInfo(iBuildingBonusReq) != None:
			bonusTechReq = GC.getBonusInfo(iBuildingBonusReq).getTechCityTrade()
			if GC.getTechInfo(bonusTechReq) != None:
				bonusTechLoc = GC.getTechInfo(bonusTechReq).getGridX()
			else:
				bonusTechLoc = 0
			if bonusTechLoc > iTechLoc:
				print CvBuildingInfo.getType()+" - Singular AND bonus prereq late!"

		#<VicinityBonus>BONUS_X
		iBuildingVicinityBonusReq = CvBuildingInfo.getPrereqVicinityBonus()
		if GC.getBonusInfo(iBuildingVicinityBonusReq) != None:
			bonusTechReq = GC.getBonusInfo(iBuildingVicinityBonusReq).getTechCityTrade()
			if GC.getTechInfo(bonusTechReq) != None:
				bonusTechLoc = GC.getTechInfo(bonusTechReq).getGridX()
			else:
				bonusTechLoc = 0
			if bonusTechLoc > iTechLoc:
				print CvBuildingInfo.getType()+" - Singular AND vicinity bonus prereq late!"

		#<RawVicinityBonus>BONUS_X
		iBuildingRawVicinityBonusReq = CvBuildingInfo.getPrereqRawVicinityBonus()
		if GC.getBonusInfo(iBuildingRawVicinityBonusReq) != None:
			bonusTechReq = GC.getBonusInfo(iBuildingRawVicinityBonusReq).getTechReveal()
			if GC.getTechInfo(bonusTechReq) != None:
				bonusTechLoc = GC.getTechInfo(bonusTechReq).getGridX()
			else:
				bonusTechLoc = 0
			if bonusTechLoc > iTechLoc:
				print CvBuildingInfo.getType()+" - Singular AND raw vicinity bonus prereq late!"

		#<PrereqBonuses>
		bonusTechLocList = []
		for bonusOr in CvBuildingInfo.getPrereqOrBonuses():
			bonusTechReq = GC.getBonusInfo(bonusOr).getTechCityTrade()
			if bonusTechReq > -1:
				bonusTechLocList.append(GC.getTechInfo(bonusTechReq).getGridX())
			else:
				bonusTechLocList.append(0)
		if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
			print CvBuildingInfo.getType()+" - Earliest OR bonus prereq late!"

		#<PrereqVicinityBonuses>
		bonusTechLocList = []
		for bonusOrVic in CvBuildingInfo.getPrereqOrVicinityBonuses():
			bonusTechReq = GC.getBonusInfo(bonusOrVic).getTechCityTrade()
			if bonusTechReq > -1:
				bonusTechLocList.append(GC.getTechInfo(bonusTechReq).getGridX())
			else:
				bonusTechLocList.append(0)
		if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
			print CvBuildingInfo.getType()+" - Earliest OR vicinity bonus prereq late!"

		#<PrereqRawVicinityBonuses>
		bonusTechLocList = []
		for bonusOrVicRaw in CvBuildingInfo.getPrereqOrRawVicinityBonuses():
			bonusTechReq = GC.getBonusInfo(bonusOrVicRaw).getTechReveal()
			if bonusTechReq > -1:
				bonusTechLocList.append(GC.getTechInfo(bonusTechReq).getGridX())
			else:
				bonusTechLocList.append(0)
		if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
			print CvBuildingInfo.getType()+" - Earliest OR raw vicinity bonus prereq late!"