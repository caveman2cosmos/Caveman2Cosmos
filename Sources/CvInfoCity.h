#ifndef CV_INFO_CITY_H
#define CV_INFO_CITY_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialistInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialistInfo : public CvHotkeyInfo
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpecialistInfo();
	virtual ~CvSpecialistInfo();

	int getGreatPeopleUnitClass() const;		// Exposed to Python
	int getGreatPeopleRateChange() const;		// Exposed to Python
	int getMissionType() const;					// Exposed to Python
	void setMissionType(int iNewType);
	// Afforess 03/26/10
	int getHealthPercent() const;
	int getHappinessPercent() const;
	bool isSlave() const;						//Exposed to Python

	int getExperience() const;					// Exposed to Python

	CvPropertyManipulators* getPropertyManipulators();

	bool isVisible() const;						// Exposed to Python

	// Arrays

	int getYieldChange(int i) const;			// Exposed to Python
	const int* getYieldChangeArray() const;		// Exposed to Python - For Moose - CvWidgetData
	int getCommerceChange(int i) const;			// Exposed to Python
	int getFlavorValue(int i) const;			// Exposed to Python

	const TCHAR* getTexture() const;			// Exposed to Python
	void setTexture(const TCHAR* szVal);

	//Team Project (1)
	//TB Specialist Tags
	// int
	int getInsidiousness() const;
	int getInvestigation() const;
	// int vector utilizing struct with delayed resolution
	int getNumTechHappinessTypes() const;
	TechModifier& getTechHappinessType(int iTech);

	int getNumTechHealthTypes() const;
	TechModifier& getTechHealthType(int iTech);

	int getNumUnitCombatExperienceTypes() const;
	UnitCombatModifier& getUnitCombatExperienceType(int iUnitCombat, bool bForLoad = true);

	//TB Specialist Tags end

	void read(FDataStreamBase* ) {}
	void write(FDataStreamBase* ) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpecialistInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);
	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iGreatPeopleUnitClass;
	int m_iGreatPeopleRateChange;
	int m_iMissionType;
	// Afforess 03/26/10
	int m_iHealthPercent;
	int m_iHappinessPercent;
	
	int m_iExperience;
	//TB Specialist Tags
	int m_iInsidiousness;
	int m_iInvestigation;
	
	// Afforess 03/26/10
	bool m_bSlave;

	bool m_bVisible;

	// Arrays
	int* m_piYieldChange;
	int* m_piCommerceChange;
	int* m_piFlavorValue;

	//Team Project (1)
	//TB Specialist Tags
	// int vector utilizing struct with delayed resolution
	std::vector<TechModifier> m_aTechHappinessTypes;
	std::vector<TechModifier> m_aTechHealthTypes;
	std::vector<UnitCombatModifier> m_aUnitCombatExperienceTypes;
	std::vector<UnitCombatModifier> m_aUnitCombatExperienceTypesNull;

	CvPropertyManipulators m_PropertyManipulators;
	CvString m_szTexture;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialBuildingInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialBuildingInfo :
	public CvInfoBase
{

	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpecialBuildingInfo();
	virtual ~CvSpecialBuildingInfo();

	int getObsoleteTech( void ) const;					// Exposed to Python
	int getTechPrereq( void ) const;						// Exposed to Python
	int getTechPrereqAnyone( void ) const;						// Exposed to Python
	int getMaxPlayerInstances() const;

	bool isValid( void ) const;									// Exposed to Python

	// Arrays

	//int getProductionTraits(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	void copyNonDefaults(CvSpecialBuildingInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iObsoleteTech;
	int m_iTechPrereq;
	int m_iTechPrereqAnyone;
	int m_iMaxPlayerInstances;

	bool m_bValid;

	// Arrays

	//int* m_piProductionTraits;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBuildingClassInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildingClassInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBuildingClassInfo();
	virtual ~CvBuildingClassInfo();

	int getMaxGlobalInstances() const;				// Exposed to Python
	int getMaxTeamInstances() const;				// Exposed to Python
	int getMaxPlayerInstances() const;				// Exposed to Python
	int getExtraPlayerInstances() const;				// Exposed to Python
	int getDefaultBuildingIndex() const;				// Exposed to Python
	void setDefaultBuildingIndex(int i);

	bool isNoLimit() const;				// Exposed to Python
	bool isMonument() const;				// Exposed to Python

	// Arrays
	int getVictoryThreshold(int i) const;				// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvBuildingClassInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);
	bool readPass3();
	void getCheckSum(unsigned int& iSum);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iMaxPlayerInstances;
	int m_iExtraPlayerInstances;
	int m_iDefaultBuildingIndex;

	bool m_bNoLimit;
	bool m_bMonument;

	// Arrays

	int* m_piVictoryThreshold;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvHurryInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHurryInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
	public:

		CvHurryInfo();
		virtual ~CvHurryInfo();

		int getGoldPerProduction() const;		// Exposed to Python
		int getProductionPerPopulation() const;	// Exposed to Python

		bool isAnger() const;					// Exposed to Python

		void read(FDataStreamBase* stream) {}
		void write(FDataStreamBase* stream) {}

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvHurryInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
		void getCheckSum(unsigned int& iSum);

	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
	protected:

		int m_iGoldPerProduction;
		int m_iProductionPerPopulation;

		bool m_bAnger;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvProcessInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProcessInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvProcessInfo();
	virtual ~CvProcessInfo();

	int getTechPrereq() const;				// Exposed to Python

	// Arrays

	int getProductionToCommerceModifier(int i) const;	// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvProcessInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iTechPrereq;

	// Arrays
	int* m_paiProductionToCommerceModifier;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEmphasizeInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEmphasizeInfo :
	public CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvEmphasizeInfo();
	virtual ~CvEmphasizeInfo();

	bool isAvoidGrowth() const;				// Exposed to Python
	bool isGreatPeople() const;				// Exposed to Python
	// Afforess 02/10/10
	bool isAvoidAngryCitizens() const;		// Exposed to Python
	bool isAvoidUnhealthyCitizens() const;	// Exposed to Python

	// Arrays
	int getYieldChange(int i) const;		// Exposed to Python
	int getCommerceChange(int i) const;		// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvEmphasizeInfo* pClassInfo, CvXMLLoadUtility* pXML);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	bool m_bAvoidGrowth;
	bool m_bGreatPeople;
	// Afforess 02/10/10
	bool m_bAvoidAngryCitizens;
	bool m_bAvoidUnhealthyCitizens;

	// Arrays
	int* m_piYieldModifiers;
	int* m_piCommerceModifiers;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUpkeepInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUpkeepInfo :
	public CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvUpkeepInfo();
	virtual ~CvUpkeepInfo();

	int getPopulationPercent() const;		//	Exposed to Python
	int getCityPercent() const;				//	Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvUpkeepInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iPopulationPercent;
	int m_iCityPercent;

};


#endif /* CV_INFO_CITY_H */

