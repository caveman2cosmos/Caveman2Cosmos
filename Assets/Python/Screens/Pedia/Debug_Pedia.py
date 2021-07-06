#Debug functions 

from CvPythonExtensions import *
GC = CyGlobalContext()

class Debug:

	def __init__(self):
		self.bDebug = True

	#Building tech location
	def checkBuildingTechRequirementLocation(self, CvBuildingInfo):
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
		
	#Unit tech location
	def checkUnitTechRequirementLocation(self, CvUnitInfo):
		#Main tech
		TechReq = CvUnitInfo.getPrereqAndTech()
		if TechReq != -1:
			iTechMainLoc = GC.getTechInfo(TechReq).getGridX()
			iTechMainRow = GC.getTechInfo(TechReq).getGridY()
		else:
			iTechMainLoc = 0
			iTechMainRow = 0

		#Tech Type requirement
		TechTypeLocList = []
		TechTypeRowList = []
		for techType in CvUnitInfo.getPrereqAndTechs():
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

		#Pick most advanced tech
		iTechLoc = max(iTechMainLoc, iTechTypeLoc)
		if iTechLoc == iTechMainLoc:
			iTechRow = iTechMainRow
		if iTechLoc == iTechTypeLoc:
			iTechRow = iTechTypeRow
			
		return iTechLoc, iTechRow
		
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
			
		return iTechLoc, iTechRow
		
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
			
		return iTechRevealLoc, iTechRevealRow, iTechEnableLoc, iTechEnableRow
	
	#Building bonus requirements
	def checkBonusRequirements(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
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
			
	#Building requirements of buildings
	def checkBuildingRequirements(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			#<PrereqInCityBuildings> - require all buildings in list
			BuildingTechLocList = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvBuildingInfo.isPrereqInCityBuilding(iBuilding):
					BuildingInCityReqTechLoc = self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0]
					BuildingTechLocList.append(BuildingInCityReqTechLoc)
			if len(BuildingTechLocList) > 0 and max(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				print CvBuildingInfo.getType()+" is unlocked before its AND building requirements "+str(BuildingTechLocList)+" "+str(iTechLoc)

			#<PrereqOrBuildings> - require one building in list
			BuildingTechLocList = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvBuildingInfo.isPrereqOrBuilding(iBuilding):
					BuildingOrReqTechLoc = self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0]
					BuildingTechLocList.append(BuildingOrReqTechLoc)
			if len(BuildingTechLocList) > 0 and min(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				print CvBuildingInfo.getType()+" is unlocked before its earliest OR building requirement "+str(BuildingTechLocList)+" "+str(iTechLoc)

			#<PrereqAmountBuildings> - require all buildings in empire in list
			BuildingTechLocList = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvBuildingInfo.getPrereqNumOfBuilding(iBuilding):
					BuildingEmpireReqTechLoc = self.checkBuildingTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0]
					BuildingTechLocList.append(BuildingEmpireReqTechLoc)
			if len(BuildingTechLocList) > 0 and max(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				print CvBuildingInfo.getType()+" is unlocked before its Empire AND requirement "+str(BuildingTechLocList)+" "+str(iTechLoc)
			
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
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvBuildingInfo.isPrereqInCityBuilding(iBuilding):
					BuildingInCityReqTechObs = GC.getBuildingInfo(iBuilding).getObsoleteTech()
					BuildingInCityReqName = GC.getBuildingInfo(iBuilding).getType()
					BuildingInCityReqTechObsLoc = 999
					if BuildingInCityReqTechObs != -1:
						BuildingInCityReqTechObsLoc = GC.getTechInfo(BuildingInCityReqTechObs).getGridX()
					BuildingRequirementObsoleteTechLoc.append(BuildingInCityReqTechObsLoc)
					BuildingRequirementName.append(BuildingInCityReqName)
			if len(BuildingRequirementObsoleteTechLoc) > 0 and min(BuildingRequirementObsoleteTechLoc) < BuildingObsoleteTechLoc:
				print CvBuildingInfo.getType()+" has AND requirements obsolete before itself "+str(BuildingRequirementName)+str(BuildingRequirementObsoleteTechLoc)+" "+str(BuildingObsoleteTechLoc)

			#<PrereqOrBuildings> - require one building in list
			BuildingRequirementObsoleteTechLoc = []
			BuildingRequirementName = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvBuildingInfo.isPrereqOrBuilding(iBuilding):
					BuildingOrReqTechObs = GC.getBuildingInfo(iBuilding).getObsoleteTech()
					BuildingOrReqName = GC.getBuildingInfo(iBuilding).getType()
					BuildingOrReqTechObsLoc = 999
					if BuildingOrReqTechObs != -1:
						BuildingOrReqTechObsLoc = GC.getTechInfo(BuildingOrReqTechObs).getGridX()
					BuildingRequirementObsoleteTechLoc.append(BuildingOrReqTechObsLoc)
					BuildingRequirementName.append(BuildingOrReqName)
			if len(BuildingRequirementObsoleteTechLoc) > 0 and max(BuildingRequirementObsoleteTechLoc) < BuildingObsoleteTechLoc:
				print CvBuildingInfo.getType()+" has latest OR requirements obsolete before itself "+str(BuildingRequirementName)+str(BuildingRequirementObsoleteTechLoc)+" "+str(BuildingObsoleteTechLoc)

			#<PrereqAmountBuildings> - require all buildings in empire in list
			BuildingRequirementObsoleteTechLoc = []
			BuildingRequirementName = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvBuildingInfo.getPrereqNumOfBuilding(iBuilding):
					BuildingEmpireReqTechObs = GC.getBuildingInfo(iBuilding).getObsoleteTech()
					BuildingEmpireReqName = GC.getBuildingInfo(iBuilding).getType()
					BuildingEmpireReqTechObsLoc = 999
					if BuildingEmpireReqTechObs != -1:
						BuildingEmpireReqTechObsLoc = GC.getTechInfo(BuildingEmpireReqTechObs).getGridX()
					BuildingRequirementObsoleteTechLoc.append(BuildingEmpireReqTechObsLoc)
					BuildingRequirementName.append(BuildingEmpireReqName)
			if len(BuildingRequirementObsoleteTechLoc) > 0 and min(BuildingRequirementObsoleteTechLoc) < BuildingObsoleteTechLoc:
				print CvBuildingInfo.getType()+" has Empire AND requirements obsolete before itself "+str(BuildingRequirementName)+str(BuildingRequirementObsoleteTechLoc)+" "+str(BuildingObsoleteTechLoc)
			
	#Building tech changes and modifiers
	def checkTechMods(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			#Check if Happiness Changes techs don't appear before building can be unlocked or after is obsoleted
			for pair in CvBuildingInfo.getTechHappinessChanges():
				iTech = pair.id
				iTechTLoc = GC.getTechInfo(iTech).getGridX()
				if GC.getTechInfo(iTech) != None and iTechTLoc <= iTechLoc:
					print CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Happiness Changes early tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType()
				elif CvBuildingInfo.getObsoleteTech() != -1 and iTechTLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
					print CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Happiness Changes late tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType()

			#Check if Health Changes techs don't appear before building can be unlocked or after is obsoleted
			for pair in CvBuildingInfo.getTechHealthChanges():
				iTech = pair.id
				iTechTLoc = GC.getTechInfo(iTech).getGridX()
				if GC.getTechInfo(iTech) != None and iTechTLoc <= iTechLoc:
					print CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Health Changes early tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType()
				elif CvBuildingInfo.getObsoleteTech() != -1 and iTechTLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
					print CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Health Changes late tech: "+str(iTechTLoc)+" "+GC.getTechInfo(iTech).getType()


			#Check if Yield Changes techs don't appear before building can be unlocked or after is obsoleted
			if CvBuildingInfo.isAnyTechYieldChanges():
				iTechMod = 0
				while iTechMod < GC.getNumTechInfos():
					i = 0
					while i < YieldTypes.NUM_YIELD_TYPES:
						if CvBuildingInfo.getTechYieldChange(iTechMod, i):
							iTechMLoc = GC.getTechInfo(iTechMod).getGridX()
							if GC.getTechInfo(iTechMod) != None and iTechMLoc <= iTechLoc:
								print CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Yield Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								print CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Yield Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
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
							if GC.getTechInfo(iTechMod) != None and iTechMLoc <= iTechLoc:
								print CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Yield Modifiers early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								print CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Yield Modifiers late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
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
							if GC.getTechInfo(iTechMod) != None and iTechMLoc <= iTechLoc:
								print CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Commerce Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								print CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Commerce Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
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
							if GC.getTechInfo(iTechMod) != None and iTechMLoc <= iTechLoc:
								print CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Commerce Modifiers early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								print CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Commerce Modifiers late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
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
							if GC.getTechInfo(iTechMod) != None and iTechMLoc <= iTechLoc:
								print CvBuildingInfo.getType()+" Tech unlock: "+str(iTechLoc)+" Specialist Changes early tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
							elif CvBuildingInfo.getObsoleteTech() != -1 and iTechMLoc >= GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX():
								print CvBuildingInfo.getType()+" Tech obsolete: "+str(GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX())+" Specialist Changes late tech: "+str(iTechMLoc)+" "+GC.getTechInfo(iTechMod).getType()
						i += 1
					iTechMod += 1
	
	#Building earliest manufacturer on resource tech reveal
	def checkBonusManufacturerTech(self):
		#Index of array - Bonus ID, array value at index - producers highest tech requirement location
		bonuslist = [-1]*GC.getNumBonusInfos()
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			
			#Singular <FreeBonus>
			for bonus in xrange(GC.getNumBonusInfos()):
				if CvBuildingInfo.getFreeBonus() == bonus:
					bonusTechReq = GC.getBonusInfo(bonus).getTechCityTrade()
					if GC.getTechInfo(bonusTechReq) != None:
						bonusTechLoc = GC.getTechInfo(bonusTechReq).getGridX()
						if bonuslist[bonus] == -1:
							bonuslist[bonus] = iTechLoc
						elif bonuslist[bonus] != -1 and bonuslist[bonus] > iTechLoc:
							bonuslist[bonus] = iTechLoc

				#<ExtraFreeBonuses>
				for bonuses in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
					if CvBuildingInfo.getExtraFreeBonus(bonuses) == bonus:
						bonusTechReq = GC.getBonusInfo(bonus).getTechCityTrade()
						if GC.getTechInfo(bonusTechReq) != None:
							bonusTechLoc = GC.getTechInfo(bonusTechReq).getGridX()
							if bonuslist[bonus] == -1:
								bonuslist[bonus] = iTechLoc
							elif bonuslist[bonus] != -1 and bonuslist[bonus] > iTechLoc:
								bonuslist[bonus] = iTechLoc
								
		for bonustype in xrange(len(bonuslist)):
			if bonuslist[bonustype] != -1 and GC.getTechInfo(GC.getBonusInfo(bonustype).getTechCityTrade()) != None and not GC.getBonusInfo(bonustype).getConstAppearance() > 0:
				if bonuslist[bonustype] - GC.getTechInfo(GC.getBonusInfo(bonustype).getTechCityTrade()).getGridX() != 0:
					print GC.getBonusInfo(bonustype).getType()+" "+str(GC.getTechInfo(GC.getBonusInfo(bonustype).getTechCityTrade()).getGridX())+" Earliest bonus producer located at: "+str(bonuslist[bonustype])
					
	#Building commerce double time - wonders only, and ensure flat change exists
	def checkBuildingCommerceDoubleTime(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.getCommerceChangeDoubleTime(iCommerce) != 0 and not (isWorldWonder(i) or isNationalWonder(i) or  CvBuildingInfo.getHolyCity() != -1):
					print CvBuildingInfo.getType()+" Non-wonder has commerce change double time"
					
			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.getCommerceChangeDoubleTime(iCommerce) != 0 and CvBuildingInfo.getCommerceChange(iCommerce) == 0:
					print CvBuildingInfo.getType()+" has commerce change double time but no relevant flat commerce change"
	
	#Building owner change - ensure flat commerce change exists
	def checkCommerceChangeOriginalOwners(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvBuildingInfo.isCommerceChangeOriginalOwner(iCommerce) and CvBuildingInfo.getCommerceChange(iCommerce) == 0:
					print CvBuildingInfo.getType()+" has CommerceChangeOriginalOwners but no flat commerce change"
					
	#Building hurry modifiers works only on buildable buildings
	def checkHurryModifier(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			if CvBuildingInfo.getProductionCost() == -1 and (CvBuildingInfo.getHurryCostModifier() != 0 or CvBuildingInfo.getHurryAngerModifier() != 0):
				print CvBuildingInfo.getType()+" can't be hurried at first place"
	
	#Buildings shouldn't obsolete too fast in relation of tech unlock
	def checkBuildingUnlockObsoletion(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0]
			if CvBuildingInfo.getObsoleteTech() != -1:
				iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
				if iObsoleteTechLoc - iTechLoc <= 5:
					print CvBuildingInfo.getType()+" Unlock: "+str(iTechLoc)+" Obsoletion: "+str(iObsoleteTechLoc)+" Difference: "+str(iObsoleteTechLoc - iTechLoc)
					
	#Building replacements shouldn't obsolete too fast for sanity of beeliners
	def checkReplacementObsoletion(self):
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
					print CvBuildingInfo.getType()+": "+str(iTechLoc)+"/"+str(iObsoleteTechLoc)+" -> "+CvBuildingReplacement.getType()+": "+str(iReplacTechLoc)+"/"+str(iReplacObsoleteTechLoc)
	
	#Buildings - noncultural wonders, religious shrines and projects should have wonder movie tag, preferably in DDS format
	def checkWonderMovies(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()
			bCultural = 0
			if iSpecialBuilding != -1 and iSpecialBuilding == GC.getInfoTypeForString("SPECIALBUILDING_C2C_CULTURE"):
				bCultural = 1
			
			if isNationalWonder(i) or (isWorldWonder(i) and not bCultural) or CvBuildingInfo.getHolyCity() != -1:
				if CvBuildingInfo.getMovie() != None:
					print CvBuildingInfo.getType()+" has movie in "+CvBuildingInfo.getMovie()
				else:
					print CvBuildingInfo.getType()+" is missing a wonder movie!"
				
		for i in xrange(GC.getNumProjectInfos()):
			CvProjectInfo = GC.getProjectInfo(i)
			if not CvProjectInfo.isSpaceship():
				if CvProjectInfo.getMovieArtDef() != "":
					print CvProjectInfo.getType()+" has movie art define tag: "+CvProjectInfo.getMovieArtDef()
				else:
					print CvProjectInfo.getType()+" is missing a wonder movie!"

	#Unit - Check unit upgrades
	def CheckUnitUpgrades(self):
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
						print str(iTechLoc)+" - "+str(CvUnitInfo.getType())+"; Upgrade: "+str(upgradedTechLoc)+" - "+str(upgradedDesc)+" -> Distance: "+str(dist)+", Cost difference: "+str(costdiff)+" Upgrade of upgrade "+str(secondUpgradeList)
					elif CvUnitInfo.getNumUnitUpgrades() > 1:
						print str(iTechLoc)+" - "+str(CvUnitInfo.getType())+"; Upgrade #"+str(u+1)+"/"+str(CvUnitInfo.getNumUnitUpgrades())+": "+str(upgradedTechLoc)+" - "+str(upgradedDesc)+" -> Distance: "+str(dist)+", Cost difference: "+str(costdiff)+" Upgrade of upgrade "+str(secondUpgradeList)
						
	#Unit - Check unit bonus requirements
	def CheckUnitBonusRequirements(self):
		for i in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(i)
			iTechLoc = self.checkUnitTechRequirementLocation(CvUnitInfo)[0]
			
			#<BonusType>BONUS_X
			iUnitBonusReq = CvUnitInfo.getPrereqAndBonus()
			if GC.getBonusInfo(iUnitBonusReq) != None:
				bonusTechReq = GC.getBonusInfo(iUnitBonusReq).getTechCityTrade()
				if GC.getTechInfo(bonusTechReq) != None:
					bonusTechLoc = GC.getTechInfo(bonusTechReq).getGridX()
				else:
					bonusTechLoc = 0
				if bonusTechLoc > iTechLoc:
					print CvUnitInfo.getType()+" - Singular AND bonus prereq late!"

			#<PrereqBonuses>
			bonusTechLocList = []
			for bonusOr in CvUnitInfo.getPrereqOrBonuses():
				bonusTechReq = GC.getBonusInfo(bonusOr).getTechCityTrade()
				if bonusTechReq > -1:
					bonusTechLocList.append(GC.getTechInfo(bonusTechReq).getGridX())
				else:
					bonusTechLocList.append(0)
			if len(bonusTechLocList) > 0 and min(bonusTechLocList) > iTechLoc:
				print CvUnitInfo.getType()+" - Earliest OR bonus prereq late!"
				
	#Bonus - check improvement productivity
	def checkBonusImprovementProductivity(self):
		for iBonus in xrange(GC.getNumBonusInfos()):
			CvBonusInfo = GC.getBonusInfo(iBonus)
			#Check total productivity: from resource, improvement and improvement+resource coupling.
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
								print CvImprovementInfo.getType()+" with "+CvBonusInfo.getType()+": F/P/C -> "+str(aFinalYield)+" upgrade: "+CvImprovementUpgradeInfo.getType()+": F/P/C -> "+str((aFinalImpUpgradeYield, (aFinalImpUpgradeYield[0]-aFinalYield[0], aFinalImpUpgradeYield[1]-aFinalYield[1], aFinalImpUpgradeYield[2]-aFinalYield[2])))

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
										print CvImprovementInfo.getType()+" with "+CvBonusInfo.getType()+": F/P/C -> "+str(aFinalYield)+" Alt upgrade: "+CvImprovementAltUpgradeInfo.getType()+": F/P/C -> "+str((aFinalImpAltUpgradeYield, (aFinalImpAltUpgradeYield[0]-aFinalYield[0], aFinalImpAltUpgradeYield[1]-aFinalYield[1], aFinalImpAltUpgradeYield[2]-aFinalYield[2])))
										
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
					if CvBuildingInfo.isMapType(GC.getInfoTypeForString("MAPCATEGORY_EARTH")): # Exclude space based
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
						if CvBuildingInfo.isMapType(GC.getInfoTypeForString("MAPCATEGORY_EARTH")): # Exclude space based
							aBuildingReplacements = [] # List building replacements
							iObsoleteTechLoc = 999 # Never obsolete
							if CvBuildingInfo.getObsoleteTech() != -1:
								iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
							for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
								CvBuildingReplacement = GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(iReplacement))
								aBuildingReplacements.append(CvBuildingReplacement.getType())

							if CvBuildingInfo.getFreeBonus() == iBonus:
								print CvBonusInfo.getType()+" "+str(self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0])+"/"+str(iObsoleteTechLoc)+" Type: "+CvBuildingInfo.getType()+" Replacement: "+str(aBuildingReplacements)

							for iBonuses in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
								if CvBuildingInfo.getExtraFreeBonus(iBonuses) == iBonus:
									print CvBonusInfo.getType()+" "+str(self.checkBuildingTechRequirementLocation(CvBuildingInfo)[0])+"/"+str(iObsoleteTechLoc)+" Type: "+CvBuildingInfo.getType()+" Replacement: "+str(aBuildingReplacements)
	
	#Improvement - yield boosts should be between improvement unlock and upgrade.
	def checkImprovementTechYieldBoosts(self):
		for iImprovement in xrange(GC.getNumImprovementInfos()):
			CvImprovementInfo = GC.getImprovementInfo(iImprovement)
			iTechLoc = self.checkImprovementTechRequirementLocation(CvImprovementInfo)[0]
			
			if CvImprovementInfo.getImprovementUpgrade() != -1 or CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes() > 0 or CvImprovementInfo.getImprovementPillage() != -1: #Only those, that can upgrade, or are top of upgrade chain
				aTechBoost = []
				CvImprovementUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getImprovementUpgrade())
				iImpUpgradeTechLoc = 0
				if CvImprovementUpgradeInfo != None: # Main upgrade
					iImpUpgradeTechLoc = GC.getTechInfo(CvImprovementUpgradeInfo.getPrereqTech()).getGridX()
				for iTech in xrange(GC.getNumTechInfos()):  # Find techs, that boost base improvement
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						if CvImprovementInfo.getTechYieldChanges(iTech, iYield) != 0:
							aTechBoost.append(GC.getTechInfo(iTech).getGridX())
				if aTechBoost and iTechLoc >= min(aTechBoost):
					print CvImprovementInfo.getType()+" Xgrid: "+str(iTechLoc)+" Tech boosts location: "+str(aTechBoost)
				if iImpUpgradeTechLoc and aTechBoost and iImpUpgradeTechLoc <= max(aTechBoost):
					print CvImprovementInfo.getType()+" Xgrid: "+str(iTechLoc)+" Tech boosts location: "+str(aTechBoost)+" Upgrade: "+CvImprovementUpgradeInfo.getType()+": "+str(iImpUpgradeTechLoc)

				#Alt upgrades
				for i in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(i))
					iImpAltUpgradeTechLoc = 0
					if CvImprovementAltUpgradeInfo != None: # Alt upgrade
						iImpAltUpgradeTechLoc = GC.getTechInfo(CvImprovementAltUpgradeInfo.getPrereqTech()).getGridX()
						if iImpAltUpgradeTechLoc and aTechBoost and iImpAltUpgradeTechLoc <= max(aTechBoost):
							print CvImprovementInfo.getType()+" Xgrid: "+str(iTechLoc)+" Tech boosts location: "+str(aTechBoost)+" Alt Upgrade: "+CvImprovementAltUpgradeInfo.getType()+": "+str(iImpAltUpgradeTechLoc)

				#Improvement yield with all techs
				aBaseImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
				aBaseUpgradeImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
				aTechImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
				aTotalImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
				for iYield in xrange(YieldTypes.NUM_YIELD_TYPES): # Food, Production, Commerce
					aBaseImprovementYield[iYield] = CvImprovementInfo.getYieldChange(iYield) # Improvement yields
					for iTech in xrange(GC.getNumTechInfos()):  # Find techs, that boost base improvement
						if CvImprovementInfo.getTechYieldChanges(iTech, iYield) != 0:
							aTechImprovementYield[iYield] += CvImprovementInfo.getTechYieldChanges(iTech, iYield)
					aTotalImprovementYield[iYield] = aBaseImprovementYield[iYield] + aTechImprovementYield[iYield]
					if CvImprovementUpgradeInfo != None: # Main upgrade
						aBaseUpgradeImprovementYield[iYield] = CvImprovementUpgradeInfo.getYieldChange(iYield)

				if CvImprovementUpgradeInfo != None and (aTotalImprovementYield[0] > aBaseUpgradeImprovementYield[0] or aTotalImprovementYield[1] > aBaseUpgradeImprovementYield[1] or aTotalImprovementYield[2] > aBaseUpgradeImprovementYield[2]):
					print CvImprovementInfo.getType()+" Total F/P/C: "+str(aTotalImprovementYield)+", "+CvImprovementUpgradeInfo.getType()+" Upgrade F/P/C: "+str(aBaseUpgradeImprovementYield)

				for i in xrange(CvImprovementInfo.getNumAlternativeImprovementUpgradeTypes()):
					aBaseAltUpgradeImprovementYield = [0]*YieldTypes.NUM_YIELD_TYPES
					CvImprovementAltUpgradeInfo = GC.getImprovementInfo(CvImprovementInfo.getAlternativeImprovementUpgradeType(i))
					for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						aBaseAltUpgradeImprovementYield[iYield] = CvImprovementAltUpgradeInfo.getYieldChange(iYield)
					if aTotalImprovementYield[0] > aBaseAltUpgradeImprovementYield[0] or aTotalImprovementYield[1] > aBaseAltUpgradeImprovementYield[1] or aTotalImprovementYield[2] > aBaseAltUpgradeImprovementYield[2]:
						print CvImprovementInfo.getType()+" Total F/P/C: "+str(aTotalImprovementYield)+", "+CvImprovementAltUpgradeInfo.getType()+" Alt Upgrade F/P/C: "+str(aBaseAltUpgradeImprovementYield)
	
	#Civic - list civics and civic categories
	def listCivics(self):
		for iCivic in xrange(GC.getNumCivicInfos()):
			CvCivicInfo = GC.getCivicInfo(iCivic)
			TechReq = CvCivicInfo.getTechPrereq()
			iTechLoc = self.checkCivicTechRequirementLocation(CvImprovementInfo)[0]

			if TechReq == -1:
				TechReq = "None"
			else:
				TechReq = GC.getTechInfo(TechReq).getType()
			print GC.getCivicOptionInfo(CvCivicInfo.getCivicOptionType()).getType()+" "+CvCivicInfo.getType()+" "+TechReq+" "+str(iTechLoc)