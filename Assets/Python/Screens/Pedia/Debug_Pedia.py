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
		
	#Building bonus requirements
	def checkBonusRequirements(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkTechRequirementLocation(CvBuildingInfo)[0]
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
			iTechLoc = self.checkTechRequirementLocation(CvBuildingInfo)[0]
			#<PrereqInCityBuildings> - require all buildings in list
			BuildingTechLocList = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvBuildingInfo.isPrereqInCityBuilding(iBuilding):
					BuildingInCityReqTechLoc = self.checkTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0]
					BuildingTechLocList.append(BuildingInCityReqTechLoc)
			if len(BuildingTechLocList) > 0 and max(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				print CvBuildingInfo.getType()+" is unlocked before its AND building requirements "+str(BuildingTechLocList)+" "+str(iTechLoc)

			#<PrereqOrBuildings> - require one building in list
			BuildingTechLocList = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvBuildingInfo.isPrereqOrBuilding(iBuilding):
					BuildingOrReqTechLoc = self.checkTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0]
					BuildingTechLocList.append(BuildingOrReqTechLoc)
			if len(BuildingTechLocList) > 0 and min(BuildingTechLocList) > iTechLoc and iTechLoc > 0:
				print CvBuildingInfo.getType()+" is unlocked before its earliest OR building requirement "+str(BuildingTechLocList)+" "+str(iTechLoc)

			#<PrereqAmountBuildings> - require all buildings in empire in list
			BuildingTechLocList = []
			for iBuilding in xrange(GC.getNumBuildingInfos()):
				if CvBuildingInfo.getPrereqNumOfBuilding(iBuilding):
					BuildingEmpireReqTechLoc = self.checkTechRequirementLocation(GC.getBuildingInfo(iBuilding))[0]
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
			iTechLoc = self.checkTechRequirementLocation(CvBuildingInfo)[0]
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
		bonuslist = [-1]*GC.getNumBonusInfos()
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkTechRequirementLocation(CvBuildingInfo)[0]
			
			for bonus in xrange(GC.getNumBonusInfos()):
				if CvBuildingInfo.getFreeBonus() == bonus:
					bonusTechReq = GC.getBonusInfo(bonus).getTechCityTrade()
					if GC.getTechInfo(bonusTechReq) != None:
						bonusTechLoc = GC.getTechInfo(bonusTechReq).getGridX()
						if bonuslist[bonus] == -1:
							bonuslist[bonus] = iTechLoc
						elif bonuslist[bonus] != -1 and bonuslist[bonus] > iTechLoc:
							bonuslist[bonus] = iTechLoc

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
					
	#Building hurry modifiers work only on buildable buildings
	def checkHurryModifier(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			if CvBuildingInfo.getProductionCost() == -1 and (CvBuildingInfo.getHurryCostModifier() != 0 or CvBuildingInfo.getHurryAngerModifier() != 0):
				print CvBuildingInfo.getType()+" can't be hurried at first place"
	
	#Buildings shouldn't obsolete too fast in relation of tech unlock
	def checkBuildingUnlockObsoletion(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkTechRequirementLocation(CvBuildingInfo)[0]
			if CvBuildingInfo.getObsoleteTech() != -1:
				iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
				if iObsoleteTechLoc - iTechLoc <= 5:
					print CvBuildingInfo.getType()+" Unlock: "+str(iTechLoc)+" Obsoletion: "+str(iObsoleteTechLoc)+" Difference: "+str(iObsoleteTechLoc - iTechLoc)
					
	#Building replacements shouldn't obsolete too fast for sanity of beeliners
	def checkReplacementObsoletion(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			iTechLoc = self.checkTechRequirementLocation(CvBuildingInfo)[0]
			if CvBuildingInfo.getObsoleteTech() != -1:
				iObsoleteTechLoc = GC.getTechInfo(CvBuildingInfo.getObsoleteTech()).getGridX()
			else:
				iObsoleteTechLoc = 999
			for iReplacement in xrange(CvBuildingInfo.getNumReplacementBuilding()):
				CvBuildingReplacement = GC.getBuildingInfo(CvBuildingInfo.getReplacementBuilding(iReplacement))
				iReplacTechLoc = self.checkTechRequirementLocation(CvBuildingReplacement)[0]
				if CvBuildingReplacement.getObsoleteTech() != -1:
					iReplacObsoleteTechLoc = GC.getTechInfo(CvBuildingReplacement.getObsoleteTech()).getGridX()
				else:
					iReplacObsoleteTechLoc = 999

				if iObsoleteTechLoc <= iReplacTechLoc:
					print CvBuildingInfo.getType()+": "+str(iTechLoc)+"/"+str(iObsoleteTechLoc)+" -> "+CvBuildingReplacement.getType()+": "+str(iReplacTechLoc)+"/"+str(iReplacObsoleteTechLoc)
