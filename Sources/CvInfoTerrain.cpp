#include "CvGameCoreDLL.h"

//======================================================================================================
//					CvRiverInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRiverInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRiverInfo::CvRiverInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRiverInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRiverInfo::~CvRiverInfo()
{
}

//======================================================================================================
//					CvRiverModelInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRiverModelInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRiverModelInfo::CvRiverModelInfo() :
m_iTextureIndex(0)
{
	m_szDeltaString[0] = '\0';
	m_szConnectString[0] = '\0';
	m_szRotateString[0] = '\0';
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRiverModelInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRiverModelInfo::~CvRiverModelInfo()
{
}

const TCHAR* CvRiverModelInfo::getModelFile() const
{
	return m_szModelFile;
}

void CvRiverModelInfo::setModelFile(const TCHAR* szVal)					// The model filename
{
	m_szModelFile=szVal;
}

const TCHAR* CvRiverModelInfo::getBorderFile() const
{
	return m_szBorderFile;
}

void CvRiverModelInfo::setBorderFile(const TCHAR* szVal)					// The model filename
{
	m_szBorderFile=szVal;
}

int CvRiverModelInfo::getTextureIndex() const
{
	return m_iTextureIndex;
}

const TCHAR* CvRiverModelInfo::getDeltaString() const
{
	return m_szDeltaString;
}

const TCHAR* CvRiverModelInfo::getConnectString() const
{
	return m_szConnectString;
}

const TCHAR* CvRiverModelInfo::getRotateString() const
{
	return m_szRotateString;
}

bool CvRiverModelInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ModelFile");
	setModelFile(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"BorderFile");
	setBorderFile(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iTextureIndex, L"TextureIndex");
	pXML->GetOptionalChildXmlValByName(m_szDeltaString, L"DeltaType");
	pXML->GetOptionalChildXmlValByName(m_szConnectString, L"Connections");
	pXML->GetOptionalChildXmlValByName(m_szRotateString, L"Rotations");

	return true;
}
void CvRiverModelInfo::copyNonDefaults(CvRiverModelInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getModelFile() == cDefault) setModelFile(pClassInfo->getModelFile());
	if (getBorderFile() == cDefault) setBorderFile(pClassInfo->getBorderFile());

	if (getTextureIndex() == iDefault) m_iTextureIndex = pClassInfo->getTextureIndex();

	if (getDeltaString() == cDefault) strcpy (m_szDeltaString, (LPCTSTR) CvString::format("%s", pClassInfo->getDeltaString()));
	if (getConnectString() == cDefault) strcpy (m_szConnectString, (LPCTSTR) CvString::format("%s", pClassInfo->getConnectString()));
	if (getRotateString() == cDefault) strcpy (m_szRotateString, (LPCTSTR) CvString::format("%s", pClassInfo->getRotateString()));
}

//======================================================================================================
//					CvRouteModelInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRouteModelInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRouteModelInfo::CvRouteModelInfo() :
m_eRouteType(NO_ROUTE),
m_bAnimated(false)
{
	m_szConnectString[0] = '\0';
	m_szModelConnectString[0] = '\0';
	m_szRotateString[0] = '\0';
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRouteModelInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRouteModelInfo::~CvRouteModelInfo()
{
}

RouteTypes CvRouteModelInfo::getRouteType() const		// The route type
{
	return m_eRouteType;
}

const TCHAR* CvRouteModelInfo::getModelFile() const
{
	return m_szModelFile;
}

void CvRouteModelInfo::setModelFile(const TCHAR* szVal)				// The model filename
{
	m_szModelFile=szVal;
}

const TCHAR* CvRouteModelInfo::getLateModelFile() const
{
	return m_szLateModelFile;
}

void CvRouteModelInfo::setLateModelFile(const TCHAR* szVal)				// The model filename
{
	m_szLateModelFile=szVal;
}

const TCHAR* CvRouteModelInfo::getModelFileKey() const
{
	return m_szModelFileKey;
}

void CvRouteModelInfo::setModelFileKey(const TCHAR* szVal)				// The model filename Key
{
	m_szModelFileKey=szVal;
}

bool CvRouteModelInfo::isAnimated() const
{
	return m_bAnimated;
}

const TCHAR* CvRouteModelInfo::getConnectString() const
{
	return m_szConnectString;
}

const TCHAR* CvRouteModelInfo::getModelConnectString() const
{
	return m_szModelConnectString;
}

const TCHAR* CvRouteModelInfo::getRotateString() const
{
	return m_szRotateString;
}

bool CvRouteModelInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"ModelFile");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 11/02/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setModelFile(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, L"LateModelFile");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 11/02/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setLateModelFile(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, L"ModelFileKey");
	setModelFileKey(szTextVal);

	pXML->GetChildXmlValByName(&m_bAnimated, L"Animated");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"RouteType");
	m_eRouteType = (RouteTypes)(pXML->GetInfoClass(szTextVal));
	pXML->GetOptionalChildXmlValByName(m_szConnectString, L"Connections");
	pXML->GetOptionalChildXmlValByName(m_szModelConnectString, L"ModelConnections");
	pXML->GetOptionalChildXmlValByName(m_szRotateString, L"Rotations");

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 11/02/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvRouteModelInfo::copyNonDefaults(CvRouteModelInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getModelFile() == cDefault) setModelFile(pClassInfo->getModelFile());
	if (getLateModelFile() == cDefault) setLateModelFile(pClassInfo->getLateModelFile());
	if (getModelFileKey() == cDefault) setModelFileKey(pClassInfo->getModelFileKey());

	if (isAnimated() == bDefault) m_bAnimated = pClassInfo->isAnimated();

	if (getRouteType() == iTextDefault) m_eRouteType = pClassInfo->getRouteType();

	if (getConnectString() == cDefault) strcpy (m_szConnectString, (LPCTSTR) CvString::format("%s", pClassInfo->getConnectString()));
	if (getModelConnectString() == cDefault) strcpy (m_szModelConnectString, (LPCTSTR) CvString::format("%s", pClassInfo->getModelConnectString()));
	if (getRotateString() == cDefault) strcpy (m_szRotateString, (LPCTSTR) CvString::format("%s", pClassInfo->getRotateString()));
}

//======================================================================================================
//					CvGoodyInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvGoodyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvGoodyInfo::CvGoodyInfo() :
m_iGold(0),
m_iGoldRand1(0),
m_iGoldRand2(0),
m_iMapOffset(0),
m_iMapRange(0),
m_iMapProb(0),
m_iExperience(0),
m_iHealing(0),
m_iDamagePrereq(0),
m_iBarbarianUnitProb(0),
m_iMinBarbarians(0),
m_iUnitClassType(NO_UNITCLASS),
m_iBarbarianUnitClass(NO_UNITCLASS),
m_iEraType(NO_ERA),
m_iNotEraType(NO_ERA),
m_iResearch(0),
m_bTech(false),
m_bBad(false),
m_bNaval(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvGoodyInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvGoodyInfo::~CvGoodyInfo()
{
}

int CvGoodyInfo::getGold() const
{
	return m_iGold;
}

int CvGoodyInfo::getGoldRand1() const
{
	return m_iGoldRand1;
}

int CvGoodyInfo::getGoldRand2() const
{
	return m_iGoldRand2;
}

int CvGoodyInfo::getMapOffset() const
{
	return m_iMapOffset;
}

int CvGoodyInfo::getMapRange() const
{
	return m_iMapRange;
}

int CvGoodyInfo::getMapProb() const
{
	return m_iMapProb;
}

int CvGoodyInfo::getExperience() const
{
	return m_iExperience;
}

int CvGoodyInfo::getHealing() const
{
	return m_iHealing;
}

int CvGoodyInfo::getDamagePrereq() const
{
	return m_iDamagePrereq;
}

int CvGoodyInfo::getBarbarianUnitProb() const
{
	return m_iBarbarianUnitProb;
}

int CvGoodyInfo::getMinBarbarians() const
{
	return m_iMinBarbarians;
}

int CvGoodyInfo::getUnitClassType() const
{
	return m_iUnitClassType;
}

int CvGoodyInfo::getBarbarianUnitClass() const
{
	return m_iBarbarianUnitClass;
}

int CvGoodyInfo::getEraType() const
{
	return m_iEraType;
}

int CvGoodyInfo::getNotEraType() const
{
	return m_iNotEraType;
}

int CvGoodyInfo::getResearch() const
{
	return m_iResearch;
}

bool CvGoodyInfo::isTech() const
{
	return m_bTech;
}

bool CvGoodyInfo::isBad() const
{
	return m_bBad;
}

bool CvGoodyInfo::isNaval() const
{
	return m_bNaval;
}

int CvGoodyInfo::getMapCategoryType(int i) const
{
	return m_aiMapCategoryTypes[i];
}

int CvGoodyInfo::getNumMapCategoryTypes() const
{
	return (int)m_aiMapCategoryTypes.size();
}

bool CvGoodyInfo::isMapCategoryType(int i)
{
	if (find(m_aiMapCategoryTypes.begin(), m_aiMapCategoryTypes.end(), i) == m_aiMapCategoryTypes.end())
	{
		return false;
	}
	return true;
}

const TCHAR* CvGoodyInfo::getSound() const
{
	return m_szSound;
}

void CvGoodyInfo::setSound(const TCHAR* szVal)
{
	m_szSound=szVal;
}

bool CvGoodyInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Sound");
	setSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iGold, L"iGold");
	pXML->GetOptionalChildXmlValByName(&m_iGoldRand1, L"iGoldRand1");
	pXML->GetOptionalChildXmlValByName(&m_iGoldRand2, L"iGoldRand2");
	pXML->GetOptionalChildXmlValByName(&m_iMapOffset, L"iMapOffset");
	pXML->GetOptionalChildXmlValByName(&m_iMapRange, L"iMapRange");
	pXML->GetOptionalChildXmlValByName(&m_iMapProb, L"iMapProb");
	pXML->GetOptionalChildXmlValByName(&m_iExperience, L"iExperience");
	pXML->GetOptionalChildXmlValByName(&m_iHealing, L"iHealing");
	pXML->GetOptionalChildXmlValByName(&m_iDamagePrereq, L"iDamagePrereq");
	pXML->GetOptionalChildXmlValByName(&m_iResearch, L"iResearch");
	pXML->GetOptionalChildXmlValByName(&m_bTech, L"bTech");
	pXML->GetOptionalChildXmlValByName(&m_bBad, L"bBad");
	pXML->GetOptionalChildXmlValByName(&m_bNaval, L"bNaval");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"UnitClass");
	m_iUnitClassType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BarbarianClass");
	m_iBarbarianUnitClass = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"EraType");
	m_iEraType = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"NotEraType");
	m_iNotEraType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iBarbarianUnitProb, L"iBarbarianUnitProb");
	pXML->GetOptionalChildXmlValByName(&m_iMinBarbarians, L"iMinBarbarians");

	pXML->SetOptionalIntVector(&m_aiMapCategoryTypes, L"MapCategoryTypes");

	return true;
}

void CvGoodyInfo::copyNonDefaults(CvGoodyInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getSound() == cDefault) setSound(pClassInfo->getSound());

	if (getGold() == iDefault) m_iGold = pClassInfo->getGold();
	if (getGoldRand1() == iDefault) m_iGoldRand1 = pClassInfo->getGoldRand1();
	if (getGoldRand2() == iDefault) m_iGoldRand2 = pClassInfo->getGoldRand2();
	if (getMapOffset() == iDefault) m_iMapOffset = pClassInfo->getMapOffset();
	if (getMapRange() == iDefault) m_iMapRange = pClassInfo->getMapRange();
	if (getMapProb() == iDefault) m_iMapProb = pClassInfo->getMapProb();
	if (getExperience() == iDefault) m_iExperience = pClassInfo->getExperience();
	if (getHealing() == iDefault) m_iHealing = pClassInfo->getHealing();
	if (getDamagePrereq() == iDefault) m_iDamagePrereq = pClassInfo->getDamagePrereq();
	if (isTech() == bDefault) m_bTech = pClassInfo->isTech();
	if (isBad() == bDefault) m_bBad = pClassInfo->isBad();
	if (isNaval() == bDefault) m_bNaval = pClassInfo->isNaval();
	if (getUnitClassType() == iTextDefault) m_iUnitClassType = pClassInfo->getUnitClassType();
	if (getBarbarianUnitClass() == iTextDefault) m_iBarbarianUnitClass = pClassInfo->getBarbarianUnitClass();
	if (getEraType() == iTextDefault) m_iEraType = pClassInfo->getEraType();
	if (getNotEraType() == iTextDefault) m_iNotEraType = pClassInfo->getNotEraType();
	if (getResearch() == iDefault) m_iResearch = pClassInfo->getResearch();

	if (getBarbarianUnitProb() == iDefault) m_iBarbarianUnitProb = pClassInfo->getBarbarianUnitProb();
	if (getMinBarbarians() == iDefault) m_iMinBarbarians = pClassInfo->getMinBarbarians();

	if (getNumMapCategoryTypes() == 0)
	{
		m_aiMapCategoryTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumMapCategoryTypes(); i++)
		{
			m_aiMapCategoryTypes.push_back(pClassInfo->getMapCategoryType(i));
		}
	}
}

void CvGoodyInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iGold);
	CheckSum(iSum, m_iGoldRand1);
	CheckSum(iSum, m_iGoldRand2);
	CheckSum(iSum, m_iMapOffset);
	CheckSum(iSum, m_iMapRange);
	CheckSum(iSum, m_iMapProb);
	CheckSum(iSum, m_iExperience);
	CheckSum(iSum, m_iHealing);
	CheckSum(iSum, m_iDamagePrereq);
	CheckSum(iSum, m_iBarbarianUnitProb);
	CheckSum(iSum, m_iMinBarbarians);
	CheckSum(iSum, m_iUnitClassType);
	CheckSum(iSum, m_iBarbarianUnitClass);
	CheckSum(iSum, m_iEraType);
	CheckSum(iSum, m_iNotEraType);
	CheckSum(iSum, m_iResearch);

	CheckSum(iSum, m_bTech);
	CheckSum(iSum, m_bBad);
	CheckSum(iSum, m_bNaval);

	CheckSumC(iSum, m_aiMapCategoryTypes);
}

//======================================================================================================
//					CvRouteInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRouteInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRouteInfo::CvRouteInfo() :
m_iAdvancedStartCost(100),
m_iAdvancedStartCostIncrease(0),
m_iValue(0),
m_iMovementCost(0),
m_iFlatMovementCost(0),
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
m_bSeaTunnel(false),
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/
m_iPrereqBonus(NO_BONUS),
m_bAnyPrereqOrBonus(false),
m_piYieldChange(NULL),
m_piTechMovementChange(NULL),
m_piPrereqOrBonuses(NULL),
m_PropertyManipulators()
{
	m_zobristValue = GC.getGameINLINE().getSorenRand().getInt();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRouteInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRouteInfo::~CvRouteInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piTechMovementChange);
	SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
}

int CvRouteInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

int CvRouteInfo::getAdvancedStartCostIncrease() const
{
	return m_iAdvancedStartCostIncrease;
}

int CvRouteInfo::getValue() const
{
	return m_iValue;
}

int CvRouteInfo::getMovementCost() const
{
	return m_iMovementCost;
}

int CvRouteInfo::getFlatMovementCost() const
{
	return m_iFlatMovementCost;
}
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvRouteInfo::isSeaTunnel() const
{
	return m_bSeaTunnel;
}
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/

int CvRouteInfo::getPrereqBonus() const
{
	return m_iPrereqBonus;
}

bool CvRouteInfo::isAnyPrereqOrBonus() const
{
	return m_bAnyPrereqOrBonus;
}
// Arrays

int CvRouteInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvRouteInfo::getYieldChangeArray()
{
	return m_piYieldChange;
}

int CvRouteInfo::getTechMovementChange(int i) const
{
	FAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piTechMovementChange ? m_piTechMovementChange[i] : 0;
}

int CvRouteInfo::getPrereqOrBonus(int i) const
{
	return m_piPrereqOrBonuses ? m_piPrereqOrBonuses[i] : -1;
}

bool CvRouteInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	//shouldHaveType = true;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	if (m_szType.empty())
	{
		OutputDebugStringW(pXML->GetXmlTagName());
		FAssert(false);
	}
	//shouldHaveType = false;

	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCost, L"iAdvancedStartCost", 100);
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCostIncrease, L"iAdvancedStartCostIncrease");

	pXML->GetOptionalChildXmlValByName(&m_iValue, L"iValue");
	pXML->GetOptionalChildXmlValByName(&m_iMovementCost, L"iMovement");
	pXML->GetOptionalChildXmlValByName(&m_iFlatMovementCost, L"iFlatMovement");
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bSeaTunnel, L"bSeaTunnel");
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusType");
	m_iPrereqBonus = pXML->GetInfoClass(szTextVal);

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"Yields"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piYieldChange);
		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	pXML->SetVariableListTagPair(&m_piTechMovementChange, L"TechMovementChanges", GC.getNumTechInfos());

	if (pXML->TryMoveToXmlFirstChild(L"PrereqOrBonuses"))
	{
		//int iNumChildren = pXML->GetXmlChildrenNumber();
		FAssertMsg((0 < GC.getNUM_ROUTE_PREREQ_OR_BONUSES()) ,"Allocating zero or less memory in SetGlobalUnitInfo");
		FAssertMsg((pXML->GetXmlChildrenNumber() <= GC.getNUM_ROUTE_PREREQ_OR_BONUSES()) ,"There are more siblings than memory allocated for them in SetGlobalUnitInfo");

		if (0 < pXML->GetXmlChildrenNumber())
		{
			pXML->CvXMLLoadUtility::InitList(&m_piPrereqOrBonuses, GC.getNUM_ROUTE_PREREQ_OR_BONUSES(), -1);
			if (pXML->TryMoveToXmlFirstChild())
			{
				int j = 0;
				do
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						OutputDebugString(szTextVal.c_str());
						m_piPrereqOrBonuses[j] = pXML->GetInfoClass(szTextVal);
						m_bAnyPrereqOrBonus = true;
						++j;
						pXML->MoveToXmlParent();
					}
				}
				while (pXML->TryMoveToXmlNextSibling());

				pXML->MoveToXmlParent();
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
		}

		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
	}

	m_PropertyManipulators.read(pXML);


	return true;
}

void CvRouteInfo::copyNonDefaults(CvRouteInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getAdvancedStartCost() == 100) m_iAdvancedStartCost = pClassInfo->getAdvancedStartCost();
	if (getAdvancedStartCostIncrease() == iDefault) m_iAdvancedStartCostIncrease = pClassInfo->getAdvancedStartCostIncrease();
	if (getValue() == iDefault) m_iValue = pClassInfo->getValue();
	if (getMovementCost() == iDefault) m_iMovementCost = pClassInfo->getMovementCost();
	if (getFlatMovementCost() == iDefault) m_iFlatMovementCost = pClassInfo->getFlatMovementCost();
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (!isSeaTunnel()) m_bSeaTunnel = pClassInfo->isSeaTunnel();
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/
	if (getPrereqBonus() == iTextDefault) m_iPrereqBonus = pClassInfo->getPrereqBonus();

	for ( int i = 0;  i < NUM_YIELD_TYPES; i++)
	{
		if (getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				pXML->InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
	}

	for ( int i = 0;  i < GC.getNumTechInfos(); i++)
	{
		if (getTechMovementChange(i) == iDefault && pClassInfo->getTechMovementChange(i) != iDefault)
		{
			if ( NULL == m_piTechMovementChange )
			{
				CvXMLLoadUtility::InitList(&m_piTechMovementChange,GC.getNumTechInfos(),iDefault);
			}
			m_piTechMovementChange[i] = pClassInfo->getTechMovementChange(i);
		}
	}

	for ( int i = 0;  i < GC.getNUM_ROUTE_PREREQ_OR_BONUSES(); i++)
	{
		if (getPrereqOrBonus(i) == iDefault)
		{
			m_piPrereqOrBonuses[i] = pClassInfo->getPrereqOrBonus(i);
		}
		if (m_piPrereqOrBonuses[i] != iDefault)
		{
			m_bAnyPrereqOrBonus = true;
		}
	}

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

void CvRouteInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iAdvancedStartCost);
	CheckSum(iSum, m_iAdvancedStartCostIncrease);

	CheckSum(iSum, m_iValue);
	CheckSum(iSum, m_iMovementCost);
	CheckSum(iSum, m_iFlatMovementCost);

	CheckSum(iSum, m_bSeaTunnel);
	CheckSum(iSum, m_iPrereqBonus);
	CheckSum(iSum, m_bAnyPrereqOrBonus);

	// Arrays

	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piTechMovementChange, GC.getNumTechInfos());
	CheckSum(iSum, m_piPrereqOrBonuses, GC.getNUM_ROUTE_PREREQ_OR_BONUSES());

	m_PropertyManipulators.getCheckSum(iSum);
}

CvPropertyManipulators* CvRouteInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}


//======================================================================================================
//					CvImprovementBonusInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvImprovementBonusInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementBonusInfo::CvImprovementBonusInfo() :
m_iDiscoverRand(0),
m_bBonusMakesValid(false),
m_bObsoleteBonusMakesValid(false),
m_bBonusTrade(false),
m_piYieldChange(NULL)
/************************************************************************************************/
/* Afforess					  Start		 01/20/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iDepletionRand(0)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvImprovementBonusInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementBonusInfo::~CvImprovementBonusInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
}

int CvImprovementBonusInfo::getDiscoverRand() const
{
	return m_iDiscoverRand;
}

bool CvImprovementBonusInfo::isBonusMakesValid() const
{
	return m_bBonusMakesValid;
}

bool CvImprovementBonusInfo::isObsoleteBonusMakesValid() const
{
	return m_bObsoleteBonusMakesValid;
}

bool CvImprovementBonusInfo::isBonusTrade() const
{
	return m_bBonusTrade;
}

int CvImprovementBonusInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

/************************************************************************************************/
/* Afforess					  Start		 01/20/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
int CvImprovementBonusInfo::getDepletionRand() const
{
	return m_iDepletionRand;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

void CvImprovementBonusInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iDiscoverRand);

	CheckSum(iSum, m_bBonusMakesValid);
	CheckSum(iSum, m_bObsoleteBonusMakesValid);
	CheckSum(iSum, m_bBonusTrade);

	CheckSum(iSum, m_iDepletionRand);

	// Arrays

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChange);
}

//======================================================================================================
//					CvImprovementInfo
//======================================================================================================

ImprovementTypes CvImprovementInfo::m_ImprovementCity = NO_IMPROVEMENT;

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvImprovementInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementInfo::CvImprovementInfo() :
m_iAdvancedStartCost(100),
m_iAdvancedStartCostIncrease(0),
m_iTilesPerGoody(0),
m_iGoodyUniqueRange(0),
m_iFeatureGrowthProbability(0),
m_iUpgradeTime(0),
m_iAirBombDefense(0),
m_iDefenseModifier(0),
m_iHappiness(0),
m_iPillageGold(0),
m_iImprovementPillage(NO_IMPROVEMENT),
m_iImprovementUpgrade(NO_IMPROVEMENT),
// Super Forts begin *XML*
m_iCulture(0),
m_iCultureRange(0),
m_iVisibilityChange(0),
m_iSeeFrom(0),
m_iUniqueRange(0),
m_bBombardable(false),
m_bUpgradeRequiresFortify(false),
// Super Forts end
// Super forts C2C adaptation
m_bIsUniversalTradeBonusProvider(false),
m_bIsZOCSource(false),
// Super forts C2C adaptation end
m_bActsAsCity(true),
m_bHillsMakesValid(false),
m_bFreshWaterMakesValid(false),
m_bRiverSideMakesValid(false),
m_bNoFreshWater(false),
m_bRequiresFlatlands(false),
m_bRequiresRiverSide(false),
m_bRequiresPeak(false),
m_bRequiresIrrigation(false),
m_bCarriesIrrigation(false),
m_bRequiresFeature(false),
m_bWater(false),
m_bGoody(false),
m_bPermanent(false),
m_bOutsideBorders(false),
m_iWorldSoundscapeScriptId(0),
m_piPrereqNatureYield(NULL),
m_piYieldChange(NULL),
m_piRiverSideYieldChange(NULL),
m_piHillsYieldChange(NULL),
m_piIrrigatedChange(NULL),
m_pbTerrainMakesValid(NULL),
m_pbFeatureMakesValid(NULL),
m_ppiTechYieldChanges(NULL),
m_ppiRouteYieldChanges(NULL),
m_paImprovementBonus(NULL)
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_bPeakMakesValid(false)
,m_iHealthPercent(0)
,m_bDepletedMine(false)
,m_iDepletionRand(0)
,m_iPrereqTech(NO_TECH)
//,m_ppiTraitYieldChanges(NULL)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
,m_PropertyManipulators()
//TB Improvements
,m_bCanMoveSeaUnits(true)
,m_bChangeRemove(false)
,m_bNotOnAnyBonus(false)
,m_bNational(false)
,m_bGlobal(false)
,m_eHighestCostBuild(NO_BUILD)
,m_iHighestCost(0)
,m_iBonusChange(NO_BONUS)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvImprovementInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementInfo::~CvImprovementInfo()
{
	int iI;

	SAFE_DELETE_ARRAY(m_piPrereqNatureYield);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverSideYieldChange);
	SAFE_DELETE_ARRAY(m_piHillsYieldChange);
	SAFE_DELETE_ARRAY(m_piIrrigatedChange);
	SAFE_DELETE_ARRAY(m_pbTerrainMakesValid);
	SAFE_DELETE_ARRAY(m_pbFeatureMakesValid);

	if (m_paImprovementBonus != NULL)
	{
		SAFE_DELETE_ARRAY(m_paImprovementBonus); // XXX make sure this isn't leaking memory...
	}

	if (m_ppiTechYieldChanges != NULL)
	{
		for (iI=0;iI<GC.getNumTechInfos();iI++)
		{
			SAFE_DELETE_ARRAY(m_ppiTechYieldChanges[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppiTechYieldChanges);
	}

	if (m_ppiRouteYieldChanges != NULL)
	{
		for (iI=0;iI<GC.getNumRouteInfos();iI++)
		{
			SAFE_DELETE_ARRAY(m_ppiRouteYieldChanges[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppiRouteYieldChanges);
	}
/************************************************************************************************/
/* Afforess					  Start		 08/26/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	//if (m_ppiTraitYieldChanges != NULL)
	//{
	//	for (iI=0;iI<GC.getNumTraitInfos();iI++)
	//	{
	//		SAFE_DELETE_ARRAY(m_ppiTraitYieldChanges[iI]);
	//	}
	//	SAFE_DELETE_ARRAY(m_ppiTraitYieldChanges);
	//}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	for (int i=0; i<(int)m_aiAlternativeImprovementUpgradeTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiAlternativeImprovementUpgradeTypes[i]));
	}
	for (int i=0; i<(int)m_aiFeatureChangeTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiFeatureChangeTypes[i]));
	}
}

int CvImprovementInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

int CvImprovementInfo::getAdvancedStartCostIncrease() const
{
	return m_iAdvancedStartCostIncrease;
}

int CvImprovementInfo::getTilesPerGoody() const
{
	return m_iTilesPerGoody;
}

int CvImprovementInfo::getGoodyUniqueRange() const
{
	return m_iGoodyUniqueRange;
}

int CvImprovementInfo::getFeatureGrowthProbability() const
{
	return m_iFeatureGrowthProbability;
}

int CvImprovementInfo::getUpgradeTime() const
{
	return m_iUpgradeTime;
}

int CvImprovementInfo::getAirBombDefense() const
{
	return m_iAirBombDefense;
}

int CvImprovementInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}

int CvImprovementInfo::getHappiness() const
{
	return m_iHappiness;
}

int CvImprovementInfo::getPillageGold() const
{
	return m_iPillageGold;
}

bool CvImprovementInfo::isOutsideBorders() const
{
	return m_bOutsideBorders;
}

int CvImprovementInfo::getImprovementPillage() const
{
	return m_iImprovementPillage;
}

void CvImprovementInfo::setImprovementPillage(int i)
{
	m_iImprovementPillage = i;
}

int CvImprovementInfo::getImprovementUpgrade() const
{
	return m_iImprovementUpgrade;
}

void CvImprovementInfo::setImprovementUpgrade(int i)
{
	m_iImprovementUpgrade = i;
}

// Super Forts begin *XML*
int CvImprovementInfo::getCulture() const
{
	return m_iCulture;
}

int CvImprovementInfo::getCultureRange() const
{
	return m_iCultureRange;
}

int CvImprovementInfo::getVisibilityChange() const
{
	return m_iVisibilityChange;
}

int CvImprovementInfo::getSeeFrom() const
{
	return m_iSeeFrom;
}

int CvImprovementInfo::getUniqueRange() const
{
	return m_iUniqueRange;
}

bool CvImprovementInfo::isBombardable() const
{
	return m_bBombardable;
}

bool CvImprovementInfo::isUpgradeRequiresFortify() const
{
	return m_bUpgradeRequiresFortify;
}
// Super Forts end
// Super forts C2C adaptation
bool CvImprovementInfo::isUniversalTradeBonusProvider() const
{
	return m_bIsUniversalTradeBonusProvider;
}

bool CvImprovementInfo::isZOCSource() const
{
	return m_bIsZOCSource;
}

// Super forts C2C adaptation end

bool CvImprovementInfo::isActsAsCity() const
{
	return m_bActsAsCity;
}

bool CvImprovementInfo::isHillsMakesValid() const
{
	return m_bHillsMakesValid;
}

bool CvImprovementInfo::isFreshWaterMakesValid() const
{
	return m_bFreshWaterMakesValid;
}

bool CvImprovementInfo::isRiverSideMakesValid() const
{
	return m_bRiverSideMakesValid;
}

bool CvImprovementInfo::isNoFreshWater() const
{
	return m_bNoFreshWater;
}

bool CvImprovementInfo::isRequiresFlatlands() const
{
	return m_bRequiresFlatlands;
}

bool CvImprovementInfo::isRequiresRiverSide() const
{
	return m_bRequiresRiverSide;
}

bool CvImprovementInfo::isRequiresPeak() const
{
	return m_bRequiresPeak;
}

bool CvImprovementInfo::isRequiresIrrigation() const
{
	return m_bRequiresIrrigation;
}

bool CvImprovementInfo::isCarriesIrrigation() const
{
	return m_bCarriesIrrigation;
}

bool CvImprovementInfo::isRequiresFeature() const
{
	return m_bRequiresFeature;
}

bool CvImprovementInfo::isWater() const
{
	return m_bWater;
}

bool CvImprovementInfo::isGoody() const
{
	return m_bGoody;
}

bool CvImprovementInfo::isPermanent() const
{
	return m_bPermanent;
}

const TCHAR* CvImprovementInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

void CvImprovementInfo::setArtDefineTag(const TCHAR* szVal)
{
	m_szArtDefineTag = szVal;
}

int CvImprovementInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}

// Arrays

int CvImprovementInfo::getPrereqNatureYield(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqNatureYield ? m_piPrereqNatureYield[i] : 0;
}

int* CvImprovementInfo::getPrereqNatureYieldArray()
{
	return m_piPrereqNatureYield;
}

int CvImprovementInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvImprovementInfo::getYieldChangeArray()
{
	return m_piYieldChange;
}

int CvImprovementInfo::getRiverSideYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piRiverSideYieldChange ? m_piRiverSideYieldChange[i] : 0;
}

int* CvImprovementInfo::getRiverSideYieldChangeArray()
{
	return m_piRiverSideYieldChange;
}

int CvImprovementInfo::getHillsYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piHillsYieldChange ? m_piHillsYieldChange[i] : 0;
}

int* CvImprovementInfo::getHillsYieldChangeArray()
{
	return m_piHillsYieldChange;
}

int CvImprovementInfo::getIrrigatedYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piIrrigatedChange ? m_piIrrigatedChange[i] : 0;
}

int* CvImprovementInfo::getIrrigatedYieldChangeArray()
{
	return m_piIrrigatedChange;
}

bool CvImprovementInfo::getTerrainMakesValid(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbTerrainMakesValid ? m_pbTerrainMakesValid[i] : false;
}

bool CvImprovementInfo::getFeatureMakesValid(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbFeatureMakesValid ? m_pbFeatureMakesValid[i] : false;
}

int CvImprovementInfo::getTechYieldChanges(int i, int j) const
{
	FAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return (m_ppiTechYieldChanges && m_ppiTechYieldChanges[i]) ? m_ppiTechYieldChanges[i][j] : 0;
}

int* CvImprovementInfo::getTechYieldChangesArray(int i)
{
	return m_ppiTechYieldChanges ? m_ppiTechYieldChanges[i] : NULL;
}

int CvImprovementInfo::getRouteYieldChanges(int i, int j) const
{
	FAssertMsg(i < GC.getNumRouteInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return (m_ppiRouteYieldChanges && m_ppiRouteYieldChanges[i]) ? m_ppiRouteYieldChanges[i][j] : 0;
}

int* CvImprovementInfo::getRouteYieldChangesArray(int i)
{
	return m_ppiRouteYieldChanges ? m_ppiRouteYieldChanges[i] : NULL;
}

int CvImprovementInfo::getImprovementBonusYield(int i, int j) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_paImprovementBonus[i].m_piYieldChange ? m_paImprovementBonus[i].getYieldChange(j) : 0;
}

bool CvImprovementInfo::isImprovementBonusMakesValid(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementBonus[i].m_bBonusMakesValid;
}

bool CvImprovementInfo::isImprovementObsoleteBonusMakesValid(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementBonus[i].m_bObsoleteBonusMakesValid;
}

bool CvImprovementInfo::isImprovementBonusTrade(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	// Super forts C2C adaptation
	return m_bIsUniversalTradeBonusProvider || m_paImprovementBonus[i].m_bBonusTrade;
	// Super forts C2C adaptation end
}

int CvImprovementInfo::getImprovementBonusDiscoverRand(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementBonus[i].m_iDiscoverRand;
}

//Vectors
int CvImprovementInfo::getMapCategoryType(int i) const
{
	return m_aiMapCategoryTypes[i];
}

int CvImprovementInfo::getNumMapCategoryTypes() const
{
	return (int)m_aiMapCategoryTypes.size();
}

bool CvImprovementInfo::isMapCategoryType(int i)
{
	FAssert (i > -1 && i < GC.getNumMapCategoryInfos()); // do not include this line if for delayed resolution
	if (find(m_aiMapCategoryTypes.begin(), m_aiMapCategoryTypes.end(), i) == m_aiMapCategoryTypes.end())
	{
		return false;
	}
	return true;
}

const TCHAR* CvImprovementInfo::getButton() const
{
/************************************************************************************************/
/* XMLCOPY								 10/25/07								MRGENIE	  */
/*																							  */
/* Catch non-existing tag																	   */
/************************************************************************************************/
	CvString cDefault = CvString::format("").GetCString();
	if (getArtDefineTag() == cDefault)
	{
		return NULL;
	}
	const CvArtInfoImprovement * pImprovementArtInfo;
	pImprovementArtInfo = getArtInfo();
	if (pImprovementArtInfo != NULL)
	{
		return pImprovementArtInfo->getButton();
	}
	return NULL;
}

const CvArtInfoImprovement* CvImprovementInfo::getArtInfo() const
{
	return ARTFILEMGR.getImprovementArtInfo(getArtDefineTag());
}

const TCHAR* CvArtInfoImprovement::getShaderNIF() const
{
	return m_szShaderNIF;
}
void CvArtInfoImprovement::setShaderNIF(const TCHAR* szDesc)
{
	m_szShaderNIF = szDesc;
}

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvImprovementInfo::isPeakMakesValid() const
{
	return m_bPeakMakesValid;
}

int CvImprovementInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}

bool CvImprovementInfo::isDepletedMine() const
{
	return m_bDepletedMine;
}

int CvImprovementInfo::getImprovementBonusDepletionRand(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementBonus[i].m_iDepletionRand;
}

int CvImprovementInfo::getDepletionRand() const
{
	return m_iDepletionRand;
}

int CvImprovementInfo::getPrereqTech() const
{
	return m_iPrereqTech;
}

//int CvImprovementInfo::getTraitYieldChanges(int i, int j) const
//{
//	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
//	FAssertMsg(j > -1, "Index out of bounds");
//	return (m_ppiTraitYieldChanges && m_ppiTraitYieldChanges[i]) ? m_ppiTraitYieldChanges[i][j] : 0;
//}

//int* CvImprovementInfo::getTraitYieldChangesArray(int i)
//{
//	return m_ppiTraitYieldChanges ? m_ppiTraitYieldChanges[i] : NULL;
//}

/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

//TB Improvements
//Object Indexes
int CvImprovementInfo::getBonusChange() const
{
	return m_iBonusChange;
}

//booleans
bool CvImprovementInfo::isCanMoveSeaUnits() const
{
	return m_bCanMoveSeaUnits;
}

bool CvImprovementInfo::isChangeRemove() const
{
	return m_bChangeRemove;
}

bool CvImprovementInfo::isNotOnAnyBonus() const
{
	return m_bNotOnAnyBonus;
}

bool CvImprovementInfo::isNational() const
{
	return m_bNational;
}

bool CvImprovementInfo::isGlobal() const
{
	return m_bGlobal;
}

int CvImprovementInfo::getAlternativeImprovementUpgradeType(int i) const
{
	return m_aiAlternativeImprovementUpgradeTypes[i];
}

int CvImprovementInfo::getNumAlternativeImprovementUpgradeTypes() const
{
	return (int)m_aiAlternativeImprovementUpgradeTypes.size();
}

bool CvImprovementInfo::isAlternativeImprovementUpgradeType(int i)
{
	if (find(m_aiAlternativeImprovementUpgradeTypes.begin(), m_aiAlternativeImprovementUpgradeTypes.end(), i) == m_aiAlternativeImprovementUpgradeTypes.end())
	{
		return false;
	}
	return true;
}

int CvImprovementInfo::getFeatureChangeType(int i) const
{
	return m_aiFeatureChangeTypes[i];
}

int CvImprovementInfo::getNumFeatureChangeTypes() const
{
	return (int)m_aiFeatureChangeTypes.size();
}

bool CvImprovementInfo::isFeatureChangeType(int i)
{
	if (find(m_aiFeatureChangeTypes.begin(), m_aiFeatureChangeTypes.end(), i) == m_aiFeatureChangeTypes.end())
	{
		return false;
	}
	return true;
}
//Post Load functions
void CvImprovementInfo::setHighestCost()
{
	BuildTypes eHighestCostBuild = NO_BUILD;
	int iHighestCost = 0;
	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		if (GC.getBuildInfo((BuildTypes)iI).getImprovement() != NO_IMPROVEMENT && GC.getBuildInfo((BuildTypes)iI).getImprovement() == GC.getInfoTypeForString(m_szType))
		{
			if (GC.getBuildInfo((BuildTypes)iI).getCost() > iHighestCost)
			{
				eHighestCostBuild = (BuildTypes)iI;
				iHighestCost = GC.getBuildInfo((BuildTypes)iI).getCost();
			}
		}
	}
	m_eHighestCostBuild = (int)eHighestCostBuild;
	m_iHighestCost = iHighestCost;
}

int CvImprovementInfo::getHighestCostBuild() const
{
	return m_eHighestCostBuild;
}

int CvImprovementInfo::getHighestCost() const
{
	return m_iHighestCost;
}

void CvImprovementInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iAdvancedStartCost);
	CheckSum(iSum, m_iAdvancedStartCostIncrease);

	CheckSum(iSum, m_iTilesPerGoody);
	CheckSum(iSum, m_iGoodyUniqueRange);
	CheckSum(iSum, m_iFeatureGrowthProbability);
	CheckSum(iSum, m_iUpgradeTime);
	CheckSum(iSum, m_iAirBombDefense);
	CheckSum(iSum, m_iDefenseModifier);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iPillageGold);
	CheckSum(iSum, m_iImprovementPillage);
	CheckSum(iSum, m_iImprovementUpgrade);

	// Super Forts begin *XML*
	CheckSum(iSum, m_iCulture);
	CheckSum(iSum, m_iCultureRange);
	CheckSum(iSum, m_iVisibilityChange);
	CheckSum(iSum, m_iSeeFrom);
	CheckSum(iSum, m_iUniqueRange);
	CheckSum(iSum, m_bBombardable);
	CheckSum(iSum, m_bUpgradeRequiresFortify);
	// Super Forts end
	// Super forts C2C adaptation
	CheckSum(iSum, m_bIsUniversalTradeBonusProvider);
	CheckSum(iSum, m_bIsZOCSource);
	// Super forts C2C adaptation end

	CheckSum(iSum, m_bActsAsCity);
	CheckSum(iSum, m_bHillsMakesValid);
	CheckSum(iSum, m_bFreshWaterMakesValid);
	CheckSum(iSum, m_bRiverSideMakesValid);
	CheckSum(iSum, m_bNoFreshWater);
	CheckSum(iSum, m_bRequiresFlatlands);
	CheckSum(iSum, m_bRequiresRiverSide);
	CheckSum(iSum, m_bRequiresPeak);
	CheckSum(iSum, m_bRequiresIrrigation);
	CheckSum(iSum, m_bCarriesIrrigation);
	CheckSum(iSum, m_bRequiresFeature);
	CheckSum(iSum, m_bWater);
	CheckSum(iSum, m_bGoody);
	CheckSum(iSum, m_bPermanent);
	CheckSum(iSum, m_bOutsideBorders);
	CheckSumC(iSum, m_aiMapCategoryTypes);

	// Arrays

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piPrereqNatureYield);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piRiverSideYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piHillsYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piIrrigatedChange);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbTerrainMakesValid);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_pbFeatureMakesValid);

	int i;
	if (m_paImprovementBonus)
		for (i = 0; i < GC.getNumBonusInfos(); i++)
		{
			m_paImprovementBonus[i].getCheckSum(iSum);
		}

	if (m_ppiTechYieldChanges)
		for(i=0;i<GC.getNumTechInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiTechYieldChanges[i]);
		}

	if (m_ppiRouteYieldChanges)
		for(i=0;i<GC.getNumRouteInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiRouteYieldChanges[i]);
		}

	CheckSum(iSum, m_iHealthPercent);
	CheckSum(iSum, m_bPeakMakesValid);
	CheckSum(iSum, m_bDepletedMine);
	CheckSum(iSum, m_iDepletionRand);
	CheckSum(iSum, m_iPrereqTech);

	//if (m_ppiTraitYieldChanges)
	//	for(i=0;i<GC.getNumTraitInfos();i++)
	//	{
	//		CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiTraitYieldChanges[i]);
	//	}

	m_PropertyManipulators.getCheckSum(iSum);
	//TB Improvements
	//Object Indexes
	CheckSum(iSum, m_iBonusChange);
	//Booleans
	CheckSum(iSum, m_bCanMoveSeaUnits);
	CheckSum(iSum, m_bChangeRemove);
	CheckSum(iSum, m_bNotOnAnyBonus);
	CheckSum(iSum, m_bNational);
	CheckSum(iSum, m_bGlobal);
	CheckSumC(iSum, m_aiAlternativeImprovementUpgradeTypes);
	CheckSumC(iSum, m_aiFeatureChangeTypes);
}

bool CvImprovementInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int iIndex, j, iNumSibs;

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ArtDefineTag");
	setArtDefineTag(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"PrereqNatureYields"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piPrereqNatureYield);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piPrereqNatureYield);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"RiverSideYieldChange"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piRiverSideYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piRiverSideYieldChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"HillsYieldChange"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piHillsYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piHillsYieldChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"IrrigatedYieldChange"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piIrrigatedChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piIrrigatedChange);
	}

	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCost, L"iAdvancedStartCost", 100);
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCostIncrease, L"iAdvancedStartCostIncrease");
	pXML->GetOptionalChildXmlValByName(&m_bActsAsCity, L"bActsAsCity", true);
	pXML->GetOptionalChildXmlValByName(&m_bHillsMakesValid, L"bHillsMakesValid");
	pXML->GetOptionalChildXmlValByName(&m_bFreshWaterMakesValid, L"bFreshWaterMakesValid");
	pXML->GetOptionalChildXmlValByName(&m_bRiverSideMakesValid, L"bRiverSideMakesValid");
	pXML->GetOptionalChildXmlValByName(&m_bNoFreshWater, L"bNoFreshWater");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresFlatlands, L"bRequiresFlatlands");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresRiverSide, L"bRequiresRiverSide");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresPeak, L"bRequiresPeak");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresIrrigation, L"bRequiresIrrigation");
	pXML->GetOptionalChildXmlValByName(&m_bCarriesIrrigation, L"bCarriesIrrigation");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresFeature, L"bRequiresFeature");
	pXML->GetOptionalChildXmlValByName(&m_bWater, L"bWater");
	pXML->GetOptionalChildXmlValByName(&m_bGoody, L"bGoody");
	pXML->GetOptionalChildXmlValByName(&m_bPermanent, L"bPermanent");
	pXML->GetOptionalChildXmlValByName(&m_iTilesPerGoody, L"iTilesPerGoody");
	pXML->GetOptionalChildXmlValByName(&m_iGoodyUniqueRange, L"iGoodyRange");
	pXML->GetOptionalChildXmlValByName(&m_iFeatureGrowthProbability, L"iFeatureGrowth");
	pXML->GetOptionalChildXmlValByName(&m_iUpgradeTime, L"iUpgradeTime");
	pXML->GetOptionalChildXmlValByName(&m_iAirBombDefense, L"iAirBombDefense");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseModifier, L"iDefenseModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iPillageGold, L"iPillageGold");
	pXML->GetOptionalChildXmlValByName(&m_bOutsideBorders, L"bOutsideBorders");
	// Super Forts begin *XML*
	pXML->GetOptionalChildXmlValByName(&m_iCulture, L"iCulture");
	pXML->GetOptionalChildXmlValByName(&m_iCultureRange, L"iCultureRange");
	pXML->GetOptionalChildXmlValByName(&m_iVisibilityChange, L"iVisibilityChange");
	pXML->GetOptionalChildXmlValByName(&m_iSeeFrom, L"iSeeFrom");
	pXML->GetOptionalChildXmlValByName(&m_iUniqueRange, L"iUnique");
	pXML->GetOptionalChildXmlValByName(&m_bBombardable, L"bBombardable");
	pXML->GetOptionalChildXmlValByName(&m_bUpgradeRequiresFortify, L"bUpgradeRequiresFortify");
	// Super Forts end
	// Super forts C2C adaptation
	pXML->GetOptionalChildXmlValByName(&m_bIsUniversalTradeBonusProvider, L"bIsUniversalTradeBonusProvider");
	pXML->GetOptionalChildXmlValByName(&m_bIsZOCSource, L"bIsZOCSource");
	// Super forts C2C adaptation end

	pXML->SetVariableListTagPair(&m_pbTerrainMakesValid, L"TerrainMakesValids", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbFeatureMakesValid, L"FeatureMakesValids", GC.getNumFeatureInfos());

	pXML->SetOptionalIntVector(&m_aiMapCategoryTypes, L"MapCategoryTypes");
	if (pXML->TryMoveToXmlFirstChild(L"BonusTypeStructs"))
	{
		// call the function that sets the bonus booleans
		pXML->SetImprovementBonuses(&m_paImprovementBonus);
		pXML->MoveToXmlParent();
	}
	else
	{
		// initialize the boolean list to the correct size and all the booleans to false
		pXML->InitImprovementBonusList(&m_paImprovementBonus, GC.getNumBonusInfos());
	}

	// initialize the boolean list to the correct size and all the booleans to false
	FAssertMsg((GC.getNumTechInfos() > 0) && (NUM_YIELD_TYPES) > 0,"either the number of tech infos is zero or less or the number of yield types is zero or less");
	if (pXML->TryMoveToXmlFirstChild(L"TechYieldChanges"))
	{
			iNumSibs = pXML->GetXmlChildrenNumber();
			if (pXML->TryMoveToXmlFirstChild())
			{
				if (0 < iNumSibs)
				{
					pXML->Init2DIntList(&m_ppiTechYieldChanges, GC.getNumTechInfos(), NUM_YIELD_TYPES);
					for (j=0;j<iNumSibs;j++)
					{
						pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
						iIndex = pXML->GetInfoClass(szTextVal);

						if (iIndex > -1)
						{
							// delete the array since it will be reallocated
							SAFE_DELETE_ARRAY(m_ppiTechYieldChanges[iIndex]);
							// if we can set the current xml node to it's next sibling
							if (pXML->TryMoveToXmlFirstChild(L"TechYields"))
							{
								// call the function that sets the yield change variable
								pXML->SetYields(&m_ppiTechYieldChanges[iIndex]);
								pXML->MoveToXmlParent();
							}
						}

						if (!pXML->TryMoveToXmlNextSibling())
						{
							break;
						}
					}
				}

				pXML->MoveToXmlParent();
			}

		pXML->MoveToXmlParent();
	}

	// initialize the boolean list to the correct size and all the booleans to false
	FAssertMsg((GC.getNumRouteInfos() > 0) && (NUM_YIELD_TYPES) > 0,"either the number of route infos is zero or less or the number of yield types is zero or less");
	if (pXML->TryMoveToXmlFirstChild(L"RouteYieldChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{

			if (0 < iNumSibs)
			{
				pXML->Init2DIntList(&m_ppiRouteYieldChanges, GC.getNumRouteInfos(), NUM_YIELD_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"RouteType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiRouteYieldChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"RouteYields"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiRouteYieldChanges[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"WorldSoundscapeAudioScript");
	if ( szTextVal.GetLength() > 0 )
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	else
		m_iWorldSoundscapeScriptId = -1;
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	int iHealth = 0;
	pXML->GetOptionalChildXmlValByName(&iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");
	m_iHealthPercent += iHealth * 100;
	pXML->GetOptionalChildXmlValByName(&m_bPeakMakesValid, L"bPeakMakesValid");
	pXML->GetOptionalChildXmlValByName(&m_bDepletedMine, L"bDepletedMine");
	pXML->GetOptionalChildXmlValByName(&m_iDepletionRand, L"iDepletionRand");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_iPrereqTech = pXML->GetInfoClass(szTextVal);

	// initialize the boolean list to the correct size and all the booleans to false
	//FAssertMsg((GC.getNumTraitInfos() > 0) && (NUM_YIELD_TYPES > 0),"either the number of trait infos is zero or less or the number of yield types is zero or less");
	//if (pXML->TryMoveToXmlFirstChild(L"TraitYieldChanges"))
	//{
	//	if (pXML->SkipToNextVal())
	//	{
	//		iNumSibs = pXML->GetXmlChildrenNumber();
	//		if (pXML->TryMoveToXmlFirstChild())
	//		{
	//			if (0 < iNumSibs)
	//			{
	//				pXML->Init2DIntList(&m_ppiTraitYieldChanges, GC.getNumTraitInfos(), NUM_YIELD_TYPES);
	//				for (j=0;j<iNumSibs;j++)
	//				{
	//					pXML->GetChildXmlValByName(szTextVal, L"TraitType");
	//					iIndex = pXML->GetInfoClass(szTextVal);

	//					if (iIndex > -1)
	//					{
	//						// delete the array since it will be reallocated
	//						SAFE_DELETE_ARRAY(m_ppiTraitYieldChanges[iIndex]);
	//						// if we can set the current xml node to it's next sibling
	//						if (pXML->TryMoveToXmlFirstChild(L"TraitYields"))
	//						{
	//							// call the function that sets the yield change variable
	//							pXML->SetYields(&m_ppiTraitYieldChanges[iIndex]);
	//							pXML->MoveToXmlParent();
	//						}
	//					}

	//					if (!pXML->TryMoveToXmlNextSibling())
	//					{
	//						break;
	//					}
	//				}
	//			}

	//			pXML->MoveToXmlParent();
	//		}
	//	}

	//	pXML->MoveToXmlParent();
	//}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	m_PropertyManipulators.read(pXML);

	//TB Improvements
	//booleans
	pXML->GetOptionalChildXmlValByName(&m_bCanMoveSeaUnits, L"bCanMoveSeaUnits");
	pXML->GetOptionalChildXmlValByName(&m_bChangeRemove, L"bChangeRemove");
	pXML->GetOptionalChildXmlValByName(&m_bNotOnAnyBonus, L"bNotOnAnyBonus");
	pXML->GetOptionalChildXmlValByName(&m_bNational, L"bNational");
	pXML->GetOptionalChildXmlValByName(&m_bGlobal, L"bGlobal");
	pXML->SetOptionalIntVectorWithDelayedResolution(m_aiAlternativeImprovementUpgradeTypes, L"AlternativeImprovementUpgradeTypes");
	pXML->SetOptionalIntVector(&m_aiFeatureChangeTypes, L"FeatureChangeTypes");

	return true;
}

bool CvImprovementInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
/************************************************************************************************/
/* XMLCOPY								 10/14/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ImprovementPillage");
	m_iImprovementPillage = GC.getInfoTypeForString(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ImprovementUpgrade");
	m_iImprovementUpgrade = GC.getInfoTypeForString(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusChange");
	m_iBonusChange = GC.getInfoTypeForString(szTextVal);

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 10/14/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvImprovementInfo::copyNonDefaults(CvImprovementInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getArtDefineTag() == cDefault) setArtDefineTag(pClassInfo->getArtDefineTag());

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (getPrereqNatureYield(i) == iDefault && pClassInfo->getPrereqNatureYield(i) != iDefault)
		{
			if ( NULL == m_piPrereqNatureYield )
			{
				CvXMLLoadUtility::InitList(&m_piPrereqNatureYield,NUM_YIELD_TYPES,iDefault);
			}
			m_piPrereqNatureYield[i] = pClassInfo->getPrereqNatureYield(i);
		}
		if (getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( NULL == m_piYieldChange )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange,NUM_YIELD_TYPES,iDefault);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
		if (getRiverSideYieldChange(i) == iDefault && pClassInfo->getRiverSideYieldChange(i) != iDefault)
		{
			if ( NULL == m_piRiverSideYieldChange )
			{
				CvXMLLoadUtility::InitList(&m_piRiverSideYieldChange,NUM_YIELD_TYPES,iDefault);
			}
			m_piRiverSideYieldChange[i] = pClassInfo->getRiverSideYieldChange(i);
		}
		if (getHillsYieldChange(i) == iDefault && pClassInfo->getHillsYieldChange(i) != iDefault)
		{
			if ( NULL == m_piHillsYieldChange )
			{
				CvXMLLoadUtility::InitList(&m_piHillsYieldChange,NUM_YIELD_TYPES,iDefault);
			}
			m_piHillsYieldChange[i] = pClassInfo->getHillsYieldChange(i);
		}
		if (getIrrigatedYieldChange(i) == iDefault && pClassInfo->getIrrigatedYieldChange(i) != iDefault)
		{
			if ( NULL == m_piIrrigatedChange )
			{
				CvXMLLoadUtility::InitList(&m_piIrrigatedChange,NUM_YIELD_TYPES,iDefault);
			}
			m_piIrrigatedChange[i] = pClassInfo->getIrrigatedYieldChange(i);
		}
	}

	if (getAdvancedStartCost() == 100) m_iAdvancedStartCost = pClassInfo->getAdvancedStartCost();
	if (getAdvancedStartCostIncrease() == iDefault) m_iAdvancedStartCostIncrease = pClassInfo->getAdvancedStartCostIncrease();
	if (getTilesPerGoody() == iDefault) m_iTilesPerGoody = pClassInfo->getTilesPerGoody();
	if (getGoodyUniqueRange() == iDefault) m_iGoodyUniqueRange = pClassInfo->getGoodyUniqueRange();
	if (getFeatureGrowthProbability() == iDefault) m_iFeatureGrowthProbability = pClassInfo->getFeatureGrowthProbability();
	if (getUpgradeTime() == iDefault) m_iUpgradeTime = pClassInfo->getUpgradeTime();
	if (getAirBombDefense() == iDefault) m_iAirBombDefense = pClassInfo->getAirBombDefense();
	if (getDefenseModifier() == iDefault) m_iDefenseModifier = pClassInfo->getDefenseModifier();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getPillageGold() == iDefault) m_iPillageGold = pClassInfo->getPillageGold();
// Super forts C2C adaptation
	if (getCulture() == iDefault) m_iCulture = pClassInfo->getCulture();
	if (getCultureRange() == iDefault) m_iCultureRange = pClassInfo->getCultureRange();
	if (getVisibilityChange() == iDefault) m_iVisibilityChange = pClassInfo->getVisibilityChange();
	if (getSeeFrom() == iDefault) m_iSeeFrom = pClassInfo->getSeeFrom();
	if (getUniqueRange() == iDefault) m_iUniqueRange = pClassInfo->getUniqueRange();
	if (isBombardable() == bDefault) m_bBombardable = pClassInfo->isBombardable();
	if (isUpgradeRequiresFortify() == bDefault) m_bUpgradeRequiresFortify = pClassInfo->isUpgradeRequiresFortify();
	if (isUniversalTradeBonusProvider() == bDefault) m_bIsUniversalTradeBonusProvider = pClassInfo->isUniversalTradeBonusProvider();
	if (isZOCSource() == bDefault) m_bIsZOCSource = pClassInfo->isZOCSource();
// Super forts C2C adaptation end
	if (isActsAsCity()) m_bActsAsCity = pClassInfo->isActsAsCity();
	if (isHillsMakesValid() == bDefault) m_bHillsMakesValid = pClassInfo->isHillsMakesValid();
	if (isFreshWaterMakesValid() == bDefault) m_bFreshWaterMakesValid = pClassInfo->isFreshWaterMakesValid();
	if (isRiverSideMakesValid() == bDefault) m_bRiverSideMakesValid = pClassInfo->isRiverSideMakesValid();
	if (isNoFreshWater() == bDefault) m_bNoFreshWater = pClassInfo->isNoFreshWater();
	if (isRequiresFlatlands() == bDefault) m_bRequiresFlatlands = pClassInfo->isRequiresFlatlands();
	if (isRequiresRiverSide() == bDefault) m_bRequiresRiverSide = pClassInfo->isRequiresRiverSide();
	if (isRequiresPeak() == bDefault) m_bRequiresPeak = pClassInfo->isRequiresPeak();
	if (isRequiresIrrigation() == bDefault) m_bRequiresIrrigation = pClassInfo->isRequiresIrrigation();
	if (isCarriesIrrigation() == bDefault) m_bCarriesIrrigation = pClassInfo->isCarriesIrrigation();
	if (isRequiresFeature() == bDefault) m_bRequiresFeature = pClassInfo->isRequiresFeature();
	if (isWater() == bDefault) m_bWater = pClassInfo->isWater();
	if (isGoody() == bDefault) m_bGoody = pClassInfo->isGoody();
	if (isPermanent() == bDefault) m_bPermanent = pClassInfo->isPermanent();
	if (isOutsideBorders() == bDefault) m_bOutsideBorders = pClassInfo->isOutsideBorders();

	for ( int i = 0; i < GC.getNumTerrainInfos(); i++)
	{
		if (getTerrainMakesValid(i) == bDefault && pClassInfo->getTerrainMakesValid(i) != bDefault)
		{
			if ( NULL == m_pbTerrainMakesValid )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrainMakesValid,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbTerrainMakesValid[i] = pClassInfo->getTerrainMakesValid(i);
		}
	}
	for ( int i = 0; i < GC.getNumFeatureInfos(); i++)
	{
		if (getFeatureMakesValid(i) == bDefault && pClassInfo->getFeatureMakesValid(i) != bDefault)
		{
			if ( NULL == m_pbFeatureMakesValid )
			{
				CvXMLLoadUtility::InitList(&m_pbFeatureMakesValid,GC.getNumFeatureInfos(),bDefault);
			}
			m_pbFeatureMakesValid[i] = pClassInfo->getFeatureMakesValid(i);
		}
	}
	for ( int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		if (m_paImprovementBonus[i].m_bBonusMakesValid == bDefault)
		{
			m_paImprovementBonus[i].m_bBonusMakesValid = pClassInfo->isImprovementBonusMakesValid(i);
		}
		if (m_paImprovementBonus[i].m_bObsoleteBonusMakesValid == bDefault)
		{
			m_paImprovementBonus[i].m_bObsoleteBonusMakesValid = pClassInfo->isImprovementObsoleteBonusMakesValid(i);
		}
		if (m_paImprovementBonus[i].m_bBonusTrade == bDefault)
		{
			m_paImprovementBonus[i].m_bBonusTrade = pClassInfo->isImprovementBonusTrade(i);
		}
		if (m_paImprovementBonus[i].m_iDiscoverRand == iDefault)
		{
			m_paImprovementBonus[i].m_iDiscoverRand = pClassInfo->getImprovementBonusDiscoverRand(i);
		}
/************************************************************************************************/
/* Afforess					  Start		 01/20/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
		if (m_paImprovementBonus[i].m_iDepletionRand == 0)
		{
			m_paImprovementBonus[i].m_iDepletionRand = pClassInfo->getImprovementBonusDepletionRand(i);
		}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (m_paImprovementBonus[i].m_piYieldChange[j] == iDefault)
			{
				m_paImprovementBonus[i].m_piYieldChange[j] = pClassInfo->getImprovementBonusYield(i,j);
			}
		}
	}
	for ( int i = 0; i < GC.getNumTechInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getTechYieldChanges(i, j) == iDefault && pClassInfo->getTechYieldChanges(i, j) != iDefault)
			{
				if ( NULL == m_ppiTechYieldChanges )
				{
					pXML->Init2DIntList(&m_ppiTechYieldChanges, GC.getNumTechInfos(), NUM_YIELD_TYPES);
				}
				else if ( NULL == m_ppiTechYieldChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiTechYieldChanges[i],NUM_YIELD_TYPES,iDefault);
				}
				m_ppiTechYieldChanges[i][j] = pClassInfo->getTechYieldChanges(i, j);
			}
		}
	}
	for ( int i = 0; i < GC.getNumRouteInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getRouteYieldChanges(i, j) == iDefault && pClassInfo->getRouteYieldChanges(i, j) != iDefault)
			{
				if ( NULL == m_ppiRouteYieldChanges )
				{
					pXML->Init2DIntList(&m_ppiRouteYieldChanges, GC.getNumRouteInfos(), NUM_YIELD_TYPES);
				}
				else if ( NULL == m_ppiRouteYieldChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiRouteYieldChanges[i],NUM_YIELD_TYPES,iDefault);
				}
				m_ppiRouteYieldChanges[i][j] = pClassInfo->getRouteYieldChanges(i, j);
			}
		}
	}

	if (m_iWorldSoundscapeScriptId == iTextDefault) m_iWorldSoundscapeScriptId = pClassInfo->getWorldSoundscapeScriptId();
	//Readpass2 stuff
	if (m_iImprovementPillage == iTextDefault) m_iImprovementPillage = pClassInfo->getImprovementPillage();
	if (m_iImprovementUpgrade == iTextDefault) m_iImprovementUpgrade = pClassInfo->getImprovementUpgrade();
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (isPeakMakesValid() == bDefault) m_bPeakMakesValid = pClassInfo->isPeakMakesValid();
	if (isDepletedMine() == bDefault) m_bDepletedMine = pClassInfo->isDepletedMine();
	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();
	if (getPrereqTech() == NO_TECH) m_iPrereqTech = pClassInfo->getPrereqTech();

	//for ( int i = 0; i < GC.getNumTraitInfos(); i++)	// "Init2DIntList" verify method
	//{
	//	for ( int j = 0; j < NUM_YIELD_TYPES; j++)
	//	{
	//		if ( getTraitYieldChanges(i, j) == iDefault && pClassInfo->getTraitYieldChanges(i, j) != iDefault)
	//		{
	//			if ( NULL == m_ppiTraitYieldChanges )
	//			{
	//				pXML->Init2DIntList(&m_ppiTraitYieldChanges, GC.getNumTraitInfos(), NUM_YIELD_TYPES);
	//			}
	//			else if ( NULL == m_ppiTraitYieldChanges[i] )
	//			{
	//				CvXMLLoadUtility::InitList(&m_ppiTraitYieldChanges[i],NUM_YIELD_TYPES,iDefault);
	//			}
	//			m_ppiTraitYieldChanges[i][j] = pClassInfo->getTraitYieldChanges(i, j);
	//		}
	//	}
	//}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	//TB Improvements
	//Object Indexes
	if (m_iBonusChange == iTextDefault) m_iBonusChange = pClassInfo->getBonusChange();
	//booleans
	if (isCanMoveSeaUnits() == bDefault) m_bCanMoveSeaUnits = pClassInfo->isCanMoveSeaUnits();
	if (isChangeRemove() == bDefault) m_bChangeRemove = pClassInfo->isChangeRemove();
	if (isNotOnAnyBonus() == bDefault) m_bNotOnAnyBonus = pClassInfo->isNotOnAnyBonus();
	if (isNational() == bDefault) m_bNational = pClassInfo->isNational();
	if (isGlobal() == bDefault) m_bGlobal = pClassInfo->isGlobal();

	if (getNumAlternativeImprovementUpgradeTypes() == 0)
	{
		int iNum = pClassInfo->getNumAlternativeImprovementUpgradeTypes();
		m_aiAlternativeImprovementUpgradeTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiAlternativeImprovementUpgradeTypes[i]), (int*)&(pClassInfo->m_aiAlternativeImprovementUpgradeTypes[i]));
		}
	}

	if (getNumFeatureChangeTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiFeatureChangeTypes, pClassInfo->m_aiFeatureChangeTypes);
	}

	if (getNumMapCategoryTypes() == 0)
	{
		m_aiMapCategoryTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumMapCategoryTypes(); i++)
		{
			m_aiMapCategoryTypes.push_back(pClassInfo->getMapCategoryType(i));
		}
	}
}

void CvImprovementInfo::copyNonDefaultsReadPass2(CvImprovementInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver)
{
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	if (bOver || m_iImprovementPillage == iTextDefault) m_iImprovementPillage = pClassInfo->getImprovementPillage();
	if (bOver || m_iImprovementUpgrade == iTextDefault) m_iImprovementUpgrade = pClassInfo->getImprovementUpgrade();
	if (bOver || m_iBonusChange == iTextDefault) m_iBonusChange = pClassInfo->getBonusChange();
}

CvPropertyManipulators* CvImprovementInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

ImprovementTypes CvImprovementInfo::getImprovementCity()
{
	return m_ImprovementCity;
}

void CvImprovementInfo::setImprovementCity(ImprovementTypes eIndex)
{
	m_ImprovementCity = eIndex;
}

//======================================================================================================
//					CvBonusClassInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBonusClassInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBonusClassInfo::CvBonusClassInfo() :
m_iUniqueRange(0)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBonusClassInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBonusClassInfo::~CvBonusClassInfo()
{
}

int CvBonusClassInfo::getUniqueRange() const
{
	return m_iUniqueRange;
}

bool CvBonusClassInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iUniqueRange, L"iUnique");

	return true;
}

void CvBonusClassInfo::copyNonDefaults(CvBonusClassInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();


	CvInfoBase::copyNonDefaults(pClassInfo, pXML);
	if (getUniqueRange() == iDefault) m_iUniqueRange = pClassInfo->getUniqueRange();
}

void CvBonusClassInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iUniqueRange);
}

//======================================================================================================
//					CvBonusInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBonusInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBonusInfo::CvBonusInfo() :
m_iBonusClassType(NO_BONUSCLASS),
m_iChar(0),
m_iTechReveal(NO_TECH),
m_iTechCityTrade(NO_TECH),
m_iTechObsolete(NO_TECH),
m_iAITradeModifier(0),
m_iAIObjective(0),
m_iHealth(0),
m_iHappiness(0),
m_iMinAreaSize(0),
m_iMinLatitude(0),
m_iMaxLatitude(90),
m_iPlacementOrder(0),
m_iConstAppearance(0),
m_iRandAppearance1(0),
m_iRandAppearance2(0),
m_iRandAppearance3(0),
m_iRandAppearance4(0),
m_iPercentPerPlayer(0),
m_iTilesPer(0),
m_iMinLandPercent(0),
m_iUniqueRange(0),
m_iGroupRange(0),
m_iGroupRand(0),
m_bOneArea(false),
m_bHills(false),
m_bFlatlands(false),
m_bNoRiverSide(false),
m_bNormalize(false),
m_piYieldChange(NULL),
m_piImprovementChange(NULL),
m_pbTerrain(NULL),
m_pbFeature(NULL),
m_pbFeatureTerrain(NULL)
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_bPeaks(false)
,m_bCurrency(false)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
,m_PropertyManipulators()
,m_tradeProvidingImprovements(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBonusInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBonusInfo::~CvBonusInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piImprovementChange);
	SAFE_DELETE_ARRAY(m_pbTerrain);
	SAFE_DELETE_ARRAY(m_pbFeature);
	SAFE_DELETE_ARRAY(m_pbFeatureTerrain);	// free memory - MT

}

int CvBonusInfo::getBonusClassType() const
{
	return m_iBonusClassType;
}

int CvBonusInfo::getChar() const
{
	return m_iChar;
}

void CvBonusInfo::setChar(int i)
{
	m_iChar = i;
}

int CvBonusInfo::getTechReveal() const
{
	return m_iTechReveal;
}

int CvBonusInfo::getTechCityTrade() const
{
	return m_iTechCityTrade;
}

int CvBonusInfo::getTechObsolete() const
{
	return m_iTechObsolete;
}

int CvBonusInfo::getAITradeModifier() const
{
	return m_iAITradeModifier;
}

int CvBonusInfo::getAIObjective() const
{
	return m_iAIObjective;
}

int CvBonusInfo::getHealth() const
{
	return m_iHealth;
}

int CvBonusInfo::getHappiness() const
{
	return m_iHappiness;
}

int CvBonusInfo::getMinAreaSize() const
{
	return m_iMinAreaSize;
}

int CvBonusInfo::getMinLatitude() const
{
	return m_iMinLatitude;
}

int CvBonusInfo::getMaxLatitude() const
{
	return m_iMaxLatitude;
}

int CvBonusInfo::getPlacementOrder() const
{
	return m_iPlacementOrder;
}

int CvBonusInfo::getConstAppearance() const
{
	return m_iConstAppearance;
}

int CvBonusInfo::getRandAppearance1() const
{
	return m_iRandAppearance1;
}

int CvBonusInfo::getRandAppearance2() const
{
	return m_iRandAppearance2;
}

int CvBonusInfo::getRandAppearance3() const
{
	return m_iRandAppearance3;
}

int CvBonusInfo::getRandAppearance4() const
{
	return m_iRandAppearance4;
}

int CvBonusInfo::getPercentPerPlayer() const
{
	return m_iPercentPerPlayer;
}

int CvBonusInfo::getTilesPer() const
{
	return m_iTilesPer;
}

int CvBonusInfo::getMinLandPercent() const
{
	return m_iMinLandPercent;
}

int CvBonusInfo::getUniqueRange() const
{
	return m_iUniqueRange;
}

int CvBonusInfo::getGroupRange() const
{
	return m_iGroupRange;
}

int CvBonusInfo::getGroupRand() const
{
	return m_iGroupRand;
}

bool CvBonusInfo::isOneArea() const
{
	return m_bOneArea;
}

bool CvBonusInfo::isHills() const
{
	return m_bHills;
}

bool CvBonusInfo::isFlatlands() const
{
	return m_bFlatlands;
}

bool CvBonusInfo::isNoRiverSide() const
{
	return m_bNoRiverSide;
}

bool CvBonusInfo::isNormalize() const
{
	return m_bNormalize;
}

const TCHAR* CvBonusInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

void CvBonusInfo::setArtDefineTag(const TCHAR* szVal)
{
	m_szArtDefineTag = szVal;
}

// Arrays

int CvBonusInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvBonusInfo::getYieldChangeArray()
{
	return m_piYieldChange;
}

int CvBonusInfo::getImprovementChange(int i) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piImprovementChange ? m_piImprovementChange[i] : 0;
}

bool CvBonusInfo::isTerrain(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbTerrain ?	m_pbTerrain[i] : false;
}

bool CvBonusInfo::isFeature(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbFeature ? m_pbFeature[i] : false;
}

bool CvBonusInfo::isFeatureTerrain(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbFeatureTerrain ?	m_pbFeatureTerrain[i] : false;
}

int CvBonusInfo::getMapCategoryType(int i) const
{
	return m_aiMapCategoryTypes[i];
}

int CvBonusInfo::getNumMapCategoryTypes() const
{
	return (int)m_aiMapCategoryTypes.size();
}

bool CvBonusInfo::isMapCategoryType(int i)
{
	FAssert (i > -1 && i < GC.getNumMapCategoryInfos()); // do not include this line if for delayed resolution
	if (find(m_aiMapCategoryTypes.begin(), m_aiMapCategoryTypes.end(), i) == m_aiMapCategoryTypes.end())
	{
		return false;
	}
	return true;
}

int CvBonusInfo::getNumAfflictionCommunicabilityTypes() const
{
	return (int)m_aAfflictionCommunicabilityTypes.size();
}

PromotionLineAfflictionModifier CvBonusInfo::getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile, bool bVicinity, bool bAccessVolume)
{
	FAssertMsg(iPromotionLine < (int)m_aAfflictionCommunicabilityTypes.size(), "Index out of bounds");
	FAssertMsg(iPromotionLine > -1, "Index out of bounds");
	if ((bWorkedTile && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bWorkedTile) ||
		(bVicinity && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bVicinity) ||
		(bAccessVolume && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bAccessVolume))
	{
		PromotionLineAfflictionModifier kMod = m_aAfflictionCommunicabilityTypes[iPromotionLine];
		kMod.iModifier = 0;
		return kMod;
	}
	return m_aAfflictionCommunicabilityTypes[iPromotionLine];
}

const TCHAR* CvBonusInfo::getButton() const
{
/************************************************************************************************/
/* XMLCOPY								 10/25/07								MRGENIE	  */
/*																							  */
/* Catch non-existing tag																	   */
/************************************************************************************************/
	CvString cDefault = CvString::format("").GetCString();
	if (getArtDefineTag() == cDefault)
	{
		return NULL;
	}
	const CvArtInfoBonus * pBonusArtInfo;
	pBonusArtInfo = getArtInfo();
	if (pBonusArtInfo != NULL)
	{
		return pBonusArtInfo->getButton();
	}
	else
	{
		return NULL;
	}
}

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvBonusInfo::isPeaks() const
{
	return m_bPeaks;
}
bool CvBonusInfo::isCurrency() const
{
	return m_bCurrency;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

void CvBonusInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iBonusClassType);
	CheckSum(iSum, m_iTechReveal);
	CheckSum(iSum, m_iTechCityTrade);
	CheckSum(iSum, m_iTechObsolete);
	CheckSum(iSum, m_iAITradeModifier);
	CheckSum(iSum, m_iAIObjective);
	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iMinAreaSize);
	CheckSum(iSum, m_iMinLatitude);
	CheckSum(iSum, m_iMaxLatitude);
	CheckSum(iSum, m_iPlacementOrder);
	CheckSum(iSum, m_iConstAppearance);
	CheckSum(iSum, m_iRandAppearance1);
	CheckSum(iSum, m_iRandAppearance2);
	CheckSum(iSum, m_iRandAppearance3);
	CheckSum(iSum, m_iRandAppearance4);
	CheckSum(iSum, m_iPercentPerPlayer);
	CheckSum(iSum, m_iTilesPer);
	CheckSum(iSum, m_iMinLandPercent);
	CheckSum(iSum, m_iUniqueRange);
	CheckSum(iSum, m_iGroupRange);
	CheckSum(iSum, m_iGroupRand);

	CheckSum(iSum, m_bOneArea);
	CheckSum(iSum, m_bHills);
	CheckSum(iSum, m_bFlatlands);
	CheckSum(iSum, m_bNoRiverSide);
	CheckSum(iSum, m_bNormalize);

	// Arrays

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChange);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementChange);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbTerrain);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_pbFeature);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbFeatureTerrain);

	CheckSumC(iSum, m_aiMapCategoryTypes);

	CheckSum(iSum, m_bPeaks);
	CheckSum(iSum, m_bCurrency);

	int iNumElements = m_aAfflictionCommunicabilityTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].iModifier);
	}

	m_PropertyManipulators.getCheckSum(iSum);
}

bool CvBonusInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName( szTextVal, L"BonusClassType");
	m_iBonusClassType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName( szTextVal, L"ArtDefineTag");
	setArtDefineTag(szTextVal);

	pXML->GetOptionalChildXmlValByName( szTextVal, L"TechReveal");
	m_iTechReveal = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName( szTextVal, L"TechCityTrade");
	m_iTechCityTrade = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName( szTextVal, L"TechObsolete");
	m_iTechObsolete = pXML->GetInfoClass(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	pXML->GetOptionalChildXmlValByName(&m_iAITradeModifier, L"iAITradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAIObjective, L"iAIObjective");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iMinAreaSize, L"iMinAreaSize");
	pXML->GetOptionalChildXmlValByName(&m_iMinLatitude, L"iMinLatitude");
	pXML->GetOptionalChildXmlValByName(&m_iMaxLatitude, L"iMaxLatitude", 90);
	pXML->GetOptionalChildXmlValByName(&m_iPlacementOrder, L"iPlacementOrder");
	pXML->GetOptionalChildXmlValByName(&m_iConstAppearance, L"iConstAppearance");

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"Rands"))
	{
		pXML->GetOptionalChildXmlValByName(&m_iRandAppearance1, L"iRandApp1");
		pXML->GetOptionalChildXmlValByName(&m_iRandAppearance2, L"iRandApp2");
		pXML->GetOptionalChildXmlValByName(&m_iRandAppearance3, L"iRandApp3");
		pXML->GetOptionalChildXmlValByName(&m_iRandAppearance4, L"iRandApp4");

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(&m_iPercentPerPlayer, L"iPlayer");
	pXML->GetOptionalChildXmlValByName(&m_iTilesPer, L"iTilesPer");
	pXML->GetOptionalChildXmlValByName(&m_iMinLandPercent, L"iMinLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iUniqueRange, L"iUnique");
	pXML->GetOptionalChildXmlValByName(&m_iGroupRange, L"iGroupRange");
	pXML->GetOptionalChildXmlValByName(&m_iGroupRand, L"iGroupRand");
	pXML->GetOptionalChildXmlValByName(&m_bOneArea, L"bArea");
	pXML->GetOptionalChildXmlValByName(&m_bHills, L"bHills");
	pXML->GetOptionalChildXmlValByName(&m_bFlatlands, L"bFlatlands");
	pXML->GetOptionalChildXmlValByName(&m_bNoRiverSide, L"bNoRiverSide");
	pXML->GetOptionalChildXmlValByName(&m_bNormalize, L"bNormalize");

	pXML->SetVariableListTagPair(&m_pbTerrain, L"TerrainBooleans", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbFeature, L"FeatureBooleans", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_pbFeatureTerrain, L"FeatureTerrainBooleans", GC.getNumTerrainInfos());

	//Vectors
	pXML->SetOptionalIntVector(&m_aiMapCategoryTypes, L"MapCategoryTypes");
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bPeaks, L"bPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bCurrency, L"bCurrency");
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/


	if(pXML->TryMoveToXmlFirstChild(L"AfflictionCommunicabilityTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AfflictionCommunicabilityType" );
		m_aAfflictionCommunicabilityTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionCommunicabilityType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictionCommunicabilityTypes[i].ePromotionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].iModifier), L"iCommunicability");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bWorkedTile), L"bWorkedTile");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bVicinity), L"bVicinity");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bAccessVolume), L"bAccessVolume");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PromotionLineType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	m_PropertyManipulators.read(pXML);

	return true;
}
void CvBonusInfo::copyNonDefaults(CvBonusInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	//this must always be in advance to the Hotkeyinfo initialization
	if (getArtDefineTag() == cDefault) setArtDefineTag(pClassInfo->getArtDefineTag());

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getBonusClassType() == iTextDefault) m_iBonusClassType = pClassInfo->getBonusClassType();
	if (getTechReveal() == iTextDefault) m_iTechReveal = pClassInfo->getTechReveal();
	if (getTechCityTrade() == iTextDefault) m_iTechCityTrade = pClassInfo->getTechCityTrade();
	if (getTechObsolete() == iTextDefault) m_iTechObsolete = pClassInfo->getTechObsolete();

	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if ( getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				pXML->InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}

			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
	}
	if (getAITradeModifier() == iTextDefault) m_iAITradeModifier = pClassInfo->getAITradeModifier();
	if (getAIObjective() == iTextDefault) m_iAIObjective = pClassInfo->getAIObjective();
	if (getHealth() == iTextDefault) m_iHealth = pClassInfo->getHealth();
	if (getHappiness() == iTextDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getMinAreaSize() == iDefault) m_iMinAreaSize = pClassInfo->getMinAreaSize();
	if (getMinLatitude() == iDefault) m_iMinLatitude = pClassInfo->getMinLatitude();
	if (getMaxLatitude() == 90) m_iMaxLatitude = pClassInfo->getMaxLatitude();
	if (getPlacementOrder() == iDefault) m_iPlacementOrder = pClassInfo->getPlacementOrder();
	if (getConstAppearance() == iDefault) m_iConstAppearance = pClassInfo->getConstAppearance();
	if (getRandAppearance1() == iDefault) m_iRandAppearance1 = pClassInfo->getRandAppearance1();
	if (getRandAppearance2() == iDefault) m_iRandAppearance2 = pClassInfo->getRandAppearance2();
	if (getRandAppearance3() == iDefault) m_iRandAppearance3 = pClassInfo->getRandAppearance3();
	if (getRandAppearance4() == iDefault) m_iRandAppearance4 = pClassInfo->getRandAppearance4();
	if (getPercentPerPlayer() == iDefault) m_iPercentPerPlayer = pClassInfo->getPercentPerPlayer();
	if (getTilesPer() == iDefault) m_iTilesPer = pClassInfo->getTilesPer();
	if (getMinLandPercent() == iDefault) m_iMinLandPercent = pClassInfo->getMinLandPercent();
	if (getUniqueRange() == iDefault) m_iUniqueRange = pClassInfo->getUniqueRange();
	if (getGroupRange() == iDefault) m_iGroupRange = pClassInfo->getGroupRange();
	if (getGroupRand() == iDefault) m_iGroupRand = pClassInfo->getGroupRand();
	if (isOneArea() == bDefault) m_bOneArea = pClassInfo->isOneArea();
	if (isHills() == bDefault) m_bHills = pClassInfo->isHills();
	if (isFlatlands() == bDefault) m_bFlatlands = pClassInfo->isFlatlands();
	if (isNoRiverSide() == bDefault) m_bNoRiverSide = pClassInfo->isNoRiverSide();
	if (isNormalize() == bDefault) m_bNormalize = pClassInfo->isNormalize();

	for ( int i = 0; i < GC.getNumTerrainInfos(); i++)
	{
		if ( isTerrain(i) == bDefault && pClassInfo->isTerrain(i) != bDefault)
		{
			if ( NULL == m_pbTerrain )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrain,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbTerrain[i] = pClassInfo->isTerrain(i);
		}
		if ( isFeatureTerrain(i) == bDefault && pClassInfo->isFeatureTerrain(i) != bDefault)
		{
			if ( NULL == m_pbFeatureTerrain )
			{
				CvXMLLoadUtility::InitList(&m_pbFeatureTerrain,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbFeatureTerrain[i] = pClassInfo->isFeatureTerrain(i);
		}
	}
	for ( int i = 0; i < GC.getNumFeatureInfos(); i++)
	{
		if ( isFeature(i) == bDefault && pClassInfo->isFeature(i) != bDefault)
		{
			if ( NULL == m_pbFeature )
			{
				CvXMLLoadUtility::InitList(&m_pbFeature,GC.getNumFeatureInfos(),bDefault);
			}
			m_pbFeature[i] = pClassInfo->isFeature(i);
		}
	}
	//Vectors
	if (getNumMapCategoryTypes() == 0)
	{
		m_aiMapCategoryTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumMapCategoryTypes(); i++)
		{
			m_aiMapCategoryTypes.push_back(pClassInfo->getMapCategoryType(i));
		}
	}
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (isPeaks() == bDefault) m_bPeaks = pClassInfo->isPeaks();
	if (isCurrency() == bDefault) m_bCurrency = pClassInfo->isCurrency();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

CvPropertyManipulators* CvBonusInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

const std::vector<std::pair<ImprovementTypes,BuildTypes> >*	CvBonusInfo::getTradeProvidingImprovements()
{
	if ( m_tradeProvidingImprovements == NULL )
	{
		int eBonus = GC.getInfoTypeForString(m_szType);

		if ( m_tradeProvidingImprovements == NULL )
		{
			std::vector<std::pair<ImprovementTypes,BuildTypes> >* tradeProvidingImprovements = new std::vector<std::pair<ImprovementTypes,BuildTypes> >();

			for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
			{
				BuildTypes eBuild = ((BuildTypes)iJ);

				if( eBuild != NO_BUILD )
				{
					ImprovementTypes eImp = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();

					if( eImp != NO_IMPROVEMENT && GC.getImprovementInfo(eImp).isImprovementBonusTrade(eBonus) )
					{
						tradeProvidingImprovements->push_back(std::make_pair(eImp, eBuild));
					}
				}
			}

			m_tradeProvidingImprovements = (volatile std::vector<std::pair<ImprovementTypes,BuildTypes> >*)tradeProvidingImprovements;
		}
	}

	return (const std::vector<std::pair<ImprovementTypes,BuildTypes> >*)m_tradeProvidingImprovements;
}


//======================================================================================================
//					CvFeatureInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvFeatureInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvFeatureInfo::CvFeatureInfo() :
m_iMovementCost(0),
m_iSeeThroughChange(0),
m_iHealthPercent(0),
m_iAppearanceProbability(0),
m_iDisappearanceProbability(0),
m_iGrowthProbability(0),
m_iDefenseModifier(0),
m_iAdvancedStartRemoveCost(0),
m_iTurnDamage(0),
m_iWarmingDefense(0), //GWMod new xml field M.A.
m_bNoCoast(false),
m_bNoRiver(false),
m_bNoAdjacent(false),
m_bRequiresFlatlands(false),
m_bRequiresRiver(false),
m_bAddsFreshWater(false),
m_bImpassable(false),
m_bNoCity(false),
m_bNoImprovement(false),
m_bVisibleAlways(false),
m_bNukeImmune(false),
m_bCountsAsPeak(false),
m_iWorldSoundscapeScriptId(0),
m_iEffectProbability(0),
m_piYieldChange(NULL),
m_piRiverYieldChange(NULL),
m_piHillsYieldChange(NULL),
m_pi3DAudioScriptFootstepIndex(NULL),
m_pbTerrain(NULL)
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iSpreadProbability(0)
,m_iCultureDistance(0)
,m_iPopDestroys(-1)
,m_bIgnoreTerrainCulture(false)
,m_bCanGrowAnywhere(false)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
,m_PropertyManipulators()
{
	m_zobristValue = GC.getGameINLINE().getSorenRand().getInt();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvFeatureInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvFeatureInfo::~CvFeatureInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverYieldChange);
	SAFE_DELETE_ARRAY(m_piHillsYieldChange);
	SAFE_DELETE_ARRAY(m_pi3DAudioScriptFootstepIndex);
	SAFE_DELETE_ARRAY(m_pbTerrain);
}

int CvFeatureInfo::getMovementCost() const
{
	return m_iMovementCost;
}

int CvFeatureInfo::getSeeThroughChange() const
{
	return m_iSeeThroughChange;
}

int CvFeatureInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}

int CvFeatureInfo::getAppearanceProbability() const
{
	return m_iAppearanceProbability;
}

int CvFeatureInfo::getDisappearanceProbability() const
{
	return m_iDisappearanceProbability;
}

int CvFeatureInfo::getGrowthProbability() const
{
	return m_iGrowthProbability;
}

int CvFeatureInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}

int CvFeatureInfo::getAdvancedStartRemoveCost() const
{
	return m_iAdvancedStartRemoveCost;
}

int CvFeatureInfo::getTurnDamage() const
{
	return m_iTurnDamage;
}

int CvFeatureInfo::getWarmingDefense() const			//GWMod new xml field M.A.
{
	return m_iWarmingDefense;
}

int CvFeatureInfo::getPopDestroys() const			//GWMod new xml field M.A.
{
	return m_iPopDestroys;
}

bool CvFeatureInfo::isNoCoast() const
{
	return m_bNoCoast;
}

bool CvFeatureInfo::isNoRiver() const
{
	return m_bNoRiver;
}

bool CvFeatureInfo::isNoAdjacent() const
{
	return m_bNoAdjacent;
}

bool CvFeatureInfo::isRequiresFlatlands() const
{
	return m_bRequiresFlatlands;
}

bool CvFeatureInfo::isRequiresRiver() const
{
	return m_bRequiresRiver;
}

bool CvFeatureInfo::isAddsFreshWater() const
{
	return m_bAddsFreshWater;
}

bool CvFeatureInfo::isImpassable() const
{
	return m_bImpassable;
}

bool CvFeatureInfo::isNoCity() const
{
	return m_bNoCity;
}

bool CvFeatureInfo::isNoImprovement() const
{
	return m_bNoImprovement;
}

bool CvFeatureInfo::isVisibleAlways() const
{
	return m_bVisibleAlways;
}

bool CvFeatureInfo::isNukeImmune() const
{
	return m_bNukeImmune;
}

bool CvFeatureInfo::isCountsAsPeak() const
{
	return m_bCountsAsPeak;
}

// BUG - City Plot Status - start
bool CvFeatureInfo::isOnlyBad() const
{
	if (getHealthPercent() > 0 || isAddsFreshWater())
	{
		return false;
	}
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (getYieldChange(iI) > 0)
		{
			return false;
		}
	}

	return true;
}
// BUG - City Plot Status - end

const TCHAR* CvFeatureInfo::getOnUnitChangeTo() const
{
	return m_szOnUnitChangeTo;
}

const TCHAR* CvFeatureInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

void CvFeatureInfo::setArtDefineTag(const TCHAR* szTag)
{
	m_szArtDefineTag = szTag;
}

int CvFeatureInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}

const TCHAR* CvFeatureInfo::getEffectType() const
{
	return m_szEffectType;
}

int CvFeatureInfo::getEffectProbability() const
{
	return m_iEffectProbability;
}

// Arrays

int CvFeatureInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int CvFeatureInfo::getRiverYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piRiverYieldChange ? m_piRiverYieldChange[i] : 0;
}

int CvFeatureInfo::getHillsYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piHillsYieldChange ? m_piHillsYieldChange[i] : 0;
}

int CvFeatureInfo::get3DAudioScriptFootstepIndex(int i) const
{
	//	FAssertMsg(i < ?, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pi3DAudioScriptFootstepIndex ? m_pi3DAudioScriptFootstepIndex[i] : -1;
}

//Vectors
int CvFeatureInfo::getMapCategoryType(int i) const
{
	return m_aiMapCategoryTypes[i];
}

int CvFeatureInfo::getNumMapCategoryTypes() const
{
	return (int)m_aiMapCategoryTypes.size();
}

bool CvFeatureInfo::isMapCategoryType(int i)
{
	FAssert (i > -1 && i < GC.getNumMapCategoryInfos()); // do not include this line if for delayed resolution
	if (find(m_aiMapCategoryTypes.begin(), m_aiMapCategoryTypes.end(), i) == m_aiMapCategoryTypes.end())
	{
		return false;
	}
	return true;
}

bool CvFeatureInfo::isTerrain(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbTerrain ? m_pbTerrain[i] : false;
}

int CvFeatureInfo::getNumVarieties() const
{
	return getArtInfo()->getNumVarieties();
}

const TCHAR* CvFeatureInfo::getButton() const
{
/************************************************************************************************/
/* XMLCOPY								 10/25/07								MRGENIE	  */
/*																							  */
/* Catch non-existing tag																	   */
/************************************************************************************************/
	CvString cDefault = CvString::format("").GetCString();
	if (getArtDefineTag() == cDefault)
	{
		return NULL;
	}
	const CvArtInfoFeature * pFeatureArtInfo;
	pFeatureArtInfo = getArtInfo();
	if (pFeatureArtInfo != NULL)
	{
		return pFeatureArtInfo->getButton();
	}
	else
	{
		return NULL;
	}
}

const CvArtInfoFeature* CvFeatureInfo::getArtInfo() const
{
	return ARTFILEMGR.getFeatureArtInfo( getArtDefineTag());
}

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
int CvFeatureInfo::getSpreadProbability() const
{
	return m_iSpreadProbability;
}

int CvFeatureInfo::getCultureDistance() const
{
	return m_iCultureDistance;
}

const TCHAR* CvFeatureInfo::getGrowthSound() const
{
	return m_szGrowthSound;
}

void CvFeatureInfo::setGrowthSound(const TCHAR* szVal)
{
	m_szGrowthSound = szVal;
}

bool CvFeatureInfo::isIgnoreTerrainCulture() const
{
	return m_bIgnoreTerrainCulture;
}

bool CvFeatureInfo::isCanGrowAnywhere() const
{
	return m_bCanGrowAnywhere;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

// AIAndy: Returns true if the feature can be displayed properly as secondary feature
bool CvFeatureInfo::canBeSecondary() const
{
	return !(getArtInfo()->isRiverArt() || (getArtInfo()->getTileArtType() != TILE_ART_TYPE_NONE));
}

int CvFeatureInfo::getNumAfflictionCommunicabilityTypes() const
{
	return (int)m_aAfflictionCommunicabilityTypes.size();
}

PromotionLineAfflictionModifier CvFeatureInfo::getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile, bool bVicinity, bool bAccessVolume)
{
	FAssertMsg(iPromotionLine < (int)m_aAfflictionCommunicabilityTypes.size(), "Index out of bounds");
	FAssertMsg(iPromotionLine > -1, "Index out of bounds");
	if ((bWorkedTile && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bWorkedTile) ||
		(bVicinity && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bVicinity) ||
		(bAccessVolume && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bAccessVolume))
	{
		PromotionLineAfflictionModifier kMod = m_aAfflictionCommunicabilityTypes[iPromotionLine];
		kMod.iModifier = 0;
		return kMod;
	}
	return m_aAfflictionCommunicabilityTypes[iPromotionLine];
}

bool CvFeatureInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName( szTextVal, L"ArtDefineTag");
	setArtDefineTag(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piYieldChange);

	if (pXML->TryMoveToXmlFirstChild(L"RiverYieldChange"))
	{
		pXML->SetYields(&m_piRiverYieldChange);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piRiverYieldChange);

	if (pXML->TryMoveToXmlFirstChild(L"HillsYieldChange"))
	{
		pXML->SetYields(&m_piHillsYieldChange);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piHillsYieldChange);

	pXML->GetOptionalChildXmlValByName(&m_iMovementCost, L"iMovement");
	pXML->GetOptionalChildXmlValByName(&m_iSeeThroughChange, L"iSeeThrough");
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseModifier, L"iDefense");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartRemoveCost, L"iAdvancedStartRemoveCost");
	pXML->GetOptionalChildXmlValByName(&m_iTurnDamage, L"iTurnDamage");
	pXML->GetOptionalChildXmlValByName(&m_iWarmingDefense, L"iWarmingDefense"); //GWMod new xml field M.A.
	pXML->GetOptionalChildXmlValByName(&m_iAppearanceProbability, L"iAppearance");
	pXML->GetOptionalChildXmlValByName(&m_iDisappearanceProbability, L"iDisappearance");
	pXML->GetOptionalChildXmlValByName(&m_iGrowthProbability, L"iGrowth");
	pXML->GetOptionalChildXmlValByName(&m_iPopDestroys, L"iPopDestroys", -1);
	pXML->GetOptionalChildXmlValByName(&m_bNoCoast, L"bNoCoast");
	pXML->GetOptionalChildXmlValByName(&m_bNoRiver, L"bNoRiver");
	pXML->GetOptionalChildXmlValByName(&m_bNoAdjacent, L"bNoAdjacent");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresFlatlands, L"bRequiresFlatlands");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresRiver, L"bRequiresRiver");
	pXML->GetOptionalChildXmlValByName(&m_bAddsFreshWater, L"bAddsFreshWater");
	pXML->GetOptionalChildXmlValByName(&m_bImpassable, L"bImpassable");
	pXML->GetOptionalChildXmlValByName(&m_bNoCity, L"bNoCity");
	pXML->GetOptionalChildXmlValByName(&m_bNoImprovement, L"bNoImprovement");
	pXML->GetOptionalChildXmlValByName(&m_bVisibleAlways, L"bVisibleAlways");
	pXML->GetOptionalChildXmlValByName(&m_bNukeImmune, L"bNukeImmune");
	pXML->GetOptionalChildXmlValByName(&m_bCountsAsPeak, L"bCountsAsPeak");
	pXML->GetOptionalChildXmlValByName(m_szOnUnitChangeTo, L"OnUnitChangeTo");

	pXML->SetVariableListTagPairForAudioScripts(&m_pi3DAudioScriptFootstepIndex, L"FootstepSounds", GC.getFootstepAudioTypes(), GC.getNumFootstepAudioTypes());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"WorldSoundscapeAudioScript");
	if ( szTextVal.GetLength() > 0 )
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	else
		m_iWorldSoundscapeScriptId = -1;

	pXML->GetOptionalChildXmlValByName(m_szEffectType, L"EffectType");
	pXML->GetOptionalChildXmlValByName(&m_iEffectProbability, L"iEffectProbability");

	pXML->SetVariableListTagPair(&m_pbTerrain, L"TerrainBooleans", GC.getNumTerrainInfos());
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iSpreadProbability, L"iSpread");
	pXML->GetOptionalChildXmlValByName(&m_iCultureDistance, L"iCultureDistance");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreTerrainCulture, L"bIgnoreTerrainCulture");
	pXML->GetOptionalChildXmlValByName(&m_bCanGrowAnywhere, L"bCanGrowAnywhere");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"GrowthSound");
	setGrowthSound(szTextVal);
	pXML->SetOptionalIntVector(&m_aiMapCategoryTypes, L"MapCategoryTypes");

	if(pXML->TryMoveToXmlFirstChild(L"AfflictionCommunicabilityTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AfflictionCommunicabilityType" );
		m_aAfflictionCommunicabilityTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionCommunicabilityType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictionCommunicabilityTypes[i].ePromotionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].iModifier), L"iCommunicability");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bWorkedTile), L"bWorkedTile");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bVicinity), L"bVicinity");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bAccessVolume), L"bAccessVolume");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PromotionLineType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	m_PropertyManipulators.read(pXML);
	return true;
}

void CvFeatureInfo::copyNonDefaults(CvFeatureInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getArtDefineTag() == cDefault) setArtDefineTag(pClassInfo->getArtDefineTag());

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				pXML->InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
		if ( getRiverYieldChange(i) == iDefault && pClassInfo->getRiverYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				pXML->InitList(&m_piRiverYieldChange, NUM_YIELD_TYPES);
			}
			m_piRiverYieldChange[i] = pClassInfo->getRiverYieldChange(i);
		}
		if ( getHillsYieldChange(i) == iDefault && pClassInfo->getHillsYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				pXML->InitList(&m_piHillsYieldChange, NUM_YIELD_TYPES);
			}
			m_piHillsYieldChange[i] = pClassInfo->getHillsYieldChange(i);
		}
	}

	if (getMovementCost() == iDefault) m_iMovementCost = pClassInfo->getMovementCost();
	if (getSeeThroughChange() == iDefault) m_iSeeThroughChange = pClassInfo->getSeeThroughChange();
	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();
	if (getDefenseModifier() == iDefault) m_iDefenseModifier = pClassInfo->getDefenseModifier();
	if (getAdvancedStartRemoveCost() == iDefault) m_iAdvancedStartRemoveCost = pClassInfo->getAdvancedStartRemoveCost();
	if (getTurnDamage() == iDefault) m_iTurnDamage = pClassInfo->getTurnDamage();
	if (getAppearanceProbability() == iDefault) m_iAppearanceProbability = pClassInfo->getAppearanceProbability();
	if (getDisappearanceProbability() == iDefault) m_iDisappearanceProbability = pClassInfo->getDisappearanceProbability();
	if (getGrowthProbability() == iDefault) m_iGrowthProbability = pClassInfo->getGrowthProbability();
	if (getPopDestroys() == -1) m_iPopDestroys = pClassInfo->getPopDestroys();
	if (isNoCoast() == bDefault) m_bNoCoast = pClassInfo->isNoCoast();
	if (isNoRiver() == bDefault) m_bNoRiver = pClassInfo->isNoRiver();
	if (isNoAdjacent() == bDefault) m_bNoAdjacent = pClassInfo->isNoAdjacent();
	if (isRequiresFlatlands() == bDefault) m_bRequiresFlatlands = pClassInfo->isRequiresFlatlands();
	if (isRequiresRiver() == bDefault) m_bRequiresRiver = pClassInfo->isRequiresRiver();
	if (isAddsFreshWater() == bDefault) m_bAddsFreshWater = pClassInfo->isAddsFreshWater();
	if (isImpassable() == bDefault) m_bImpassable = pClassInfo->isImpassable();
	if (isNoCity() == bDefault) m_bNoCity = pClassInfo->isNoCity();
	if (isNoImprovement() == bDefault) m_bNoImprovement = pClassInfo->isNoImprovement();
	if (isVisibleAlways() == bDefault) m_bVisibleAlways = pClassInfo->isVisibleAlways();
	if (isNukeImmune() == bDefault) m_bNukeImmune = pClassInfo->isNukeImmune();
	if (isCountsAsPeak() == bDefault) m_bCountsAsPeak = pClassInfo->isCountsAsPeak();

	if (getOnUnitChangeTo() == cDefault) m_szOnUnitChangeTo = pClassInfo->getOnUnitChangeTo();

	for ( int i = 0; i < GC.getNumFootstepAudioTypes(); i++ )
	{
		if ( get3DAudioScriptFootstepIndex(i) == iDefault && pClassInfo->get3DAudioScriptFootstepIndex(i) != iDefault)
		{
			if ( NULL == m_pi3DAudioScriptFootstepIndex )
			{
				CvXMLLoadUtility::InitList(&m_pi3DAudioScriptFootstepIndex,GC.getNumFootstepAudioTypes(),iDefault);
			}
			m_pi3DAudioScriptFootstepIndex[i] = pClassInfo->get3DAudioScriptFootstepIndex(i);
		}
	}
	if (getWorldSoundscapeScriptId() == iTextDefault) m_iWorldSoundscapeScriptId = pClassInfo->getWorldSoundscapeScriptId();

	if (getEffectType() == cDefault) m_szEffectType = pClassInfo->getEffectType();
	if (getEffectProbability() == iDefault) m_iEffectProbability = pClassInfo->getEffectProbability();

	for ( int i = 0; i < GC.getNumTerrainInfos(); i++ )
	{
		if ( isTerrain(i) == bDefault && pClassInfo->isTerrain(i) != bDefault )
		{
			if ( m_pbTerrain == NULL )
			{
				pXML->InitList(&m_pbTerrain, GC.getNumTerrainInfos());
			}

			m_pbTerrain[i] = pClassInfo->isTerrain(i);
		}
	}
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getSpreadProbability() == iDefault) m_iSpreadProbability = pClassInfo->getSpreadProbability();
	if (getCultureDistance() == iDefault) m_iCultureDistance = pClassInfo->getCultureDistance();
	if (!isIgnoreTerrainCulture()) m_bIgnoreTerrainCulture = pClassInfo->isIgnoreTerrainCulture();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	if (getNumMapCategoryTypes() == 0)
	{
		m_aiMapCategoryTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumMapCategoryTypes(); i++)
		{
			m_aiMapCategoryTypes.push_back(pClassInfo->getMapCategoryType(i));
		}
	}

	if (getNumAfflictionCommunicabilityTypes() == 0)
	{
		int iNum = pClassInfo->getNumAfflictionCommunicabilityTypes();
		m_aAfflictionCommunicabilityTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aAfflictionCommunicabilityTypes[i].ePromotionLine = pClassInfo->m_aAfflictionCommunicabilityTypes[i].ePromotionLine;
			m_aAfflictionCommunicabilityTypes[i].iModifier = pClassInfo->m_aAfflictionCommunicabilityTypes[i].iModifier;
			m_aAfflictionCommunicabilityTypes[i].bAccessVolume = pClassInfo->m_aAfflictionCommunicabilityTypes[i].bAccessVolume;
			m_aAfflictionCommunicabilityTypes[i].bVicinity = pClassInfo->m_aAfflictionCommunicabilityTypes[i].bVicinity;
			m_aAfflictionCommunicabilityTypes[i].bWorkedTile = pClassInfo->m_aAfflictionCommunicabilityTypes[i].bWorkedTile;
		}
	}

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

void CvFeatureInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iSpreadProbability);
	CheckSum(iSum, m_iCultureDistance);
	CheckSum(iSum, m_bIgnoreTerrainCulture);
	CheckSum(iSum, m_bCanGrowAnywhere);
	CheckSum(iSum, m_iMovementCost);
	CheckSum(iSum, m_iSeeThroughChange);
	CheckSum(iSum, m_iHealthPercent);
	CheckSum(iSum, m_iAppearanceProbability);
	CheckSum(iSum, m_iDisappearanceProbability);
	CheckSum(iSum, m_iGrowthProbability);
	CheckSum(iSum, m_iDefenseModifier);
	CheckSum(iSum, m_iAdvancedStartRemoveCost);
	CheckSum(iSum, m_iTurnDamage);
	CheckSum(iSum, m_iWarmingDefense);
	CheckSum(iSum, m_iPopDestroys);

	CheckSum(iSum, m_bNoCoast);
	CheckSum(iSum, m_bNoRiver);
	CheckSum(iSum, m_bNoAdjacent);
	CheckSum(iSum, m_bRequiresFlatlands);
	CheckSum(iSum, m_bRequiresRiver);
	CheckSum(iSum, m_bAddsFreshWater);
	CheckSum(iSum, m_bImpassable);
	CheckSum(iSum, m_bNoCity);
	CheckSum(iSum, m_bNoImprovement);
	CheckSum(iSum, m_bVisibleAlways);
	CheckSum(iSum, m_bNukeImmune);
	CheckSum(iSum, m_bCountsAsPeak);
	CheckSumC(iSum, m_szOnUnitChangeTo);
	CheckSumC(iSum, m_aiMapCategoryTypes);

	// Arrays

	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piRiverYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piHillsYieldChange, NUM_YIELD_TYPES);

	CheckSum(iSum, m_pbTerrain, GC.getNumTerrainInfos());


	int iNumElements = m_aAfflictionCommunicabilityTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].iModifier);
	}

	m_PropertyManipulators.getCheckSum(iSum);
}

CvPropertyManipulators* CvFeatureInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

//======================================================================================================
//					CvCommerceInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCommerceInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCommerceInfo::CvCommerceInfo() :
m_iChar(0),
m_iInitialPercent(0),
m_iInitialHappiness(0),
m_iAIWeightPercent(0),
m_bFlexiblePercent(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCommerceInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCommerceInfo::~CvCommerceInfo()
{
}

int CvCommerceInfo::getChar() const
{
	return m_iChar;
}

void CvCommerceInfo::setChar(int i)
{
	m_iChar = i;
}

int CvCommerceInfo::getInitialPercent() const
{
	return m_iInitialPercent;
}

int CvCommerceInfo::getInitialHappiness() const
{
	return m_iInitialHappiness;
}

int CvCommerceInfo::getAIWeightPercent() const
{
	return m_iAIWeightPercent;
}

bool CvCommerceInfo::isFlexiblePercent() const
{
	return m_bFlexiblePercent;
}

bool CvCommerceInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iInitialPercent, L"iInitialPercent");
	pXML->GetChildXmlValByName(&m_iInitialHappiness, L"iInitialHappiness");
	pXML->GetChildXmlValByName(&m_iAIWeightPercent, L"iAIWeightPercent");
	pXML->GetChildXmlValByName(&m_bFlexiblePercent, L"bFlexiblePercent");

	return true;
}

void CvCommerceInfo::copyNonDefaults(CvCommerceInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getInitialPercent() == iDefault) m_iInitialPercent = pClassInfo->getInitialPercent();
	if (getInitialHappiness() == iDefault) m_iInitialHappiness = pClassInfo->getInitialHappiness();
	if (getAIWeightPercent() == iDefault) m_iAIWeightPercent = pClassInfo->getAIWeightPercent();
	if (isFlexiblePercent() == bDefault) m_bFlexiblePercent = pClassInfo->isFlexiblePercent();

}

void CvCommerceInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iInitialPercent);
	CheckSum(iSum, m_iInitialHappiness);
	CheckSum(iSum, m_iAIWeightPercent);

	CheckSum(iSum, m_bFlexiblePercent);
}

//======================================================================================================
//					CvYieldInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvYieldInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvYieldInfo::CvYieldInfo() :
m_iChar(0),
m_iHillsChange(0),
m_iPeakChange(0),
m_iLakeChange(0),
m_iCityChange(0),
m_iPopulationChangeOffset(0),
m_iPopulationChangeDivisor(0),
m_iMinCity(0),
m_iTradeModifier(0),
m_iGoldenAgeYield(0),
m_iGoldenAgeYieldThreshold(0),
m_iAIWeightPercent(0),
m_iColorType(NO_COLOR),
m_paszSymbolPath(NULL)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvYieldInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvYieldInfo::~CvYieldInfo()
{
	SAFE_DELETE_ARRAY(m_paszSymbolPath);
}

int CvYieldInfo::getChar() const
{
	return m_iChar;
}

void CvYieldInfo::setChar(int i)
{
	m_iChar = i;
}

int CvYieldInfo::getHillsChange() const
{
	return m_iHillsChange;
}

int CvYieldInfo::getPeakChange() const
{
	return m_iPeakChange;
}

int CvYieldInfo::getLakeChange() const
{
	return m_iLakeChange;
}

int CvYieldInfo::getCityChange() const
{
	return m_iCityChange;
}

int CvYieldInfo::getPopulationChangeOffset() const
{
	return m_iPopulationChangeOffset;
}

int CvYieldInfo::getPopulationChangeDivisor() const
{
	return m_iPopulationChangeDivisor;
}

int CvYieldInfo::getMinCity() const
{
	return m_iMinCity;
}

int CvYieldInfo::getTradeModifier() const
{
	return m_iTradeModifier;
}

int CvYieldInfo::getGoldenAgeYield() const
{
	return m_iGoldenAgeYield;
}

int CvYieldInfo::getGoldenAgeYieldThreshold() const
{
	return m_iGoldenAgeYieldThreshold;
}

int CvYieldInfo::getAIWeightPercent() const
{
	return m_iAIWeightPercent;
}

int CvYieldInfo::getColorType() const
{
	return m_iColorType;
}

// Arrays

const TCHAR* CvYieldInfo::getSymbolPath(int i) const
{
	FAssertMsg(i < GC.getDefineINT("MAX_YIELD_STACK"), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszSymbolPath ? m_paszSymbolPath[i] : -1;
}

bool CvYieldInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int iNumSibs, j;

	pXML->GetOptionalChildXmlValByName(&m_iHillsChange, L"iHillsChange");
	pXML->GetOptionalChildXmlValByName(&m_iPeakChange, L"iPeakChange");
	pXML->GetOptionalChildXmlValByName(&m_iLakeChange, L"iLakeChange");
	pXML->GetOptionalChildXmlValByName(&m_iCityChange, L"iCityChange");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationChangeOffset, L"iPopulationChangeOffset");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationChangeDivisor, L"iPopulationChangeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iMinCity, L"iMinCity");
	pXML->GetOptionalChildXmlValByName(&m_iTradeModifier, L"iTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgeYield, L"iGoldenAgeYield");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgeYieldThreshold, L"iGoldenAgeYieldThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iAIWeightPercent, L"iAIWeightPercent");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ColorType");
	m_iColorType = pXML->GetInfoClass(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"SymbolPaths"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		FAssertMsg((0 < GC.getDefineINT("MAX_YIELD_STACK")) ,"Allocating zero or less memory in SetGlobalYieldInfo");
		m_paszSymbolPath = new CvString[GC.getDefineINT("MAX_YIELD_STACK")];

		if (0 < iNumSibs)
		{
			if (pXML->GetChildXmlVal(szTextVal))
			{
				FAssertMsg((iNumSibs <= GC.getDefineINT("MAX_YIELD_STACK")) ,"There are more siblings than memory allocated for them in SetGlobalYieldInfo");
				for (j=0;j<iNumSibs;j++)
				{
					m_paszSymbolPath[j] = szTextVal;
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}
	else
	{
		CvString cDefault = CvString::format("").GetCString();
		m_paszSymbolPath = new CvString[GC.getDefineINT("MAX_YIELD_STACK")];
		for ( int i = 0; i < GC.getDefineINT("MAX_YIELD_STACK"); i++)
		{
			m_paszSymbolPath[i] = cDefault;
		}
	}

	return true;
}

void CvYieldInfo::copyNonDefaults(CvYieldInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getHillsChange() == iDefault) m_iHillsChange = pClassInfo->getHillsChange();
	if (getPeakChange() == iDefault) m_iPeakChange = pClassInfo->getPeakChange();
	if (getLakeChange() == iDefault) m_iLakeChange = pClassInfo->getLakeChange();
	if (getCityChange() == iDefault) m_iCityChange = pClassInfo->getCityChange();
	if (getPopulationChangeOffset() == iDefault) m_iPopulationChangeOffset = pClassInfo->getPopulationChangeOffset();
	if (getPopulationChangeDivisor() == iDefault) m_iPopulationChangeDivisor = pClassInfo->getPopulationChangeDivisor();
	if (getMinCity() == iDefault) m_iMinCity = pClassInfo->getMinCity();
	if (getTradeModifier() == iDefault) m_iTradeModifier = pClassInfo->getTradeModifier();
	if (getGoldenAgeYield() == iDefault) m_iGoldenAgeYield = pClassInfo->getGoldenAgeYield();
	if (getGoldenAgeYieldThreshold() == iDefault) m_iGoldenAgeYieldThreshold = pClassInfo->getGoldenAgeYieldThreshold();
	if (getAIWeightPercent() == iDefault) m_iAIWeightPercent = pClassInfo->getAIWeightPercent();
	if (getColorType() == iTextDefault) m_iColorType = pClassInfo->getColorType();

	for ( int i = 0; i < GC.getDefineINT("MAX_YIELD_STACK"); i++)
	{
		if ( m_paszSymbolPath[i] == cDefault)
		{
			m_paszSymbolPath[i] = pClassInfo->getSymbolPath(i);
		}
	}
}

void CvYieldInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iHillsChange);
	CheckSum(iSum, m_iPeakChange);
	CheckSum(iSum, m_iLakeChange);
	CheckSum(iSum, m_iCityChange);
	CheckSum(iSum, m_iPopulationChangeOffset);
	CheckSum(iSum, m_iPopulationChangeDivisor);
	CheckSum(iSum, m_iMinCity);
	CheckSum(iSum, m_iTradeModifier);
	CheckSum(iSum, m_iGoldenAgeYield);
	CheckSum(iSum, m_iGoldenAgeYieldThreshold);
	CheckSum(iSum, m_iAIWeightPercent);
}

//======================================================================================================
//					CvTerrainInfo
//======================================================================================================

TerrainTypes CvTerrainInfo::m_TerrainPeak = NO_TERRAIN;
TerrainTypes CvTerrainInfo::m_TerrainHill = NO_TERRAIN;

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTerrainInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainInfo::CvTerrainInfo() :
m_iMovementCost(0),
m_iSeeFromLevel(0),
m_iSeeThroughLevel(0),
m_iBuildModifier(0),
m_iDefenseModifier(0),
m_bWater(false),
m_bImpassable(false),
m_bFound(false),
m_bFoundCoast(false),
m_bFoundFreshWater(false),
m_iWorldSoundscapeScriptId(0),
m_piYields(NULL),
m_piRiverYieldChange(NULL),
m_piHillsYieldChange(NULL),
m_pi3DAudioScriptFootstepIndex(NULL)
/************************************************************************************************/
/* Afforess					  Start		 04/21/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iCultureDistance(0)
,m_iHealthPercent(0)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
,m_PropertyManipulators(),
//TB Combat Mod begin
m_bColdDamage(false)
//TB Combat Mod end
{
	m_zobristValue = GC.getGameINLINE().getSorenRand().getInt();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTerrainInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainInfo::~CvTerrainInfo()
{
	SAFE_DELETE_ARRAY(m_piYields);
	SAFE_DELETE_ARRAY(m_piRiverYieldChange);
	SAFE_DELETE_ARRAY(m_piHillsYieldChange);
	SAFE_DELETE_ARRAY(m_pi3DAudioScriptFootstepIndex);

}

int CvTerrainInfo::getMovementCost() const
{
	return m_iMovementCost;
}

int CvTerrainInfo::getSeeFromLevel() const
{
	return m_iSeeFromLevel;
}

int CvTerrainInfo::getSeeThroughLevel() const
{
	return m_iSeeThroughLevel;
}

int CvTerrainInfo::getBuildModifier() const
{
	return m_iBuildModifier;
}

int CvTerrainInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}

bool CvTerrainInfo::isWater() const
{
	return m_bWater;
}

bool CvTerrainInfo::isImpassable() const
{
	return m_bImpassable;
}

bool CvTerrainInfo::isFound() const
{
	return m_bFound;
}

bool CvTerrainInfo::isFoundCoast() const
{
	return m_bFoundCoast;
}

bool CvTerrainInfo::isFoundFreshWater() const
{
	return m_bFoundFreshWater;
}

const TCHAR* CvTerrainInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

void CvTerrainInfo::setArtDefineTag(const TCHAR* szTag)
{
	m_szArtDefineTag = szTag;
}

int CvTerrainInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}

// Arrays

int CvTerrainInfo::getYield(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYields ? m_piYields[i] : 0;
}

int CvTerrainInfo::getRiverYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piRiverYieldChange ? m_piRiverYieldChange[i] : 0;
}

int CvTerrainInfo::getHillsYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piHillsYieldChange ? m_piHillsYieldChange[i] : 0;
}

int CvTerrainInfo::get3DAudioScriptFootstepIndex(int i) const
{
//	FAssertMsg(i < ?, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pi3DAudioScriptFootstepIndex ? m_pi3DAudioScriptFootstepIndex[i] : 0;
}
/************************************************************************************************/
/* Afforess					  Start		 04/21/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
int CvTerrainInfo::getCultureDistance() const
{
	return m_iCultureDistance;
}
int CvTerrainInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

//TB Combat Mod begin
bool CvTerrainInfo::isColdDamage() const
{
	return m_bColdDamage;
}
//TB Combat Mod end
//Vectors
int CvTerrainInfo::getMapCategoryType(int i) const
{
	return m_aiMapCategoryTypes[i];
}

int CvTerrainInfo::getNumMapCategoryTypes() const
{
	return (int)m_aiMapCategoryTypes.size();
}

bool CvTerrainInfo::isMapCategoryType(int i)
{
	FAssert (i > -1 && i < GC.getNumMapCategoryInfos()); // do not include this line if for delayed resolution
	if (find(m_aiMapCategoryTypes.begin(), m_aiMapCategoryTypes.end(), i) == m_aiMapCategoryTypes.end())
	{
		return false;
	}
	return true;
}

int CvTerrainInfo::getNumAfflictionCommunicabilityTypes() const
{
	return (int)m_aAfflictionCommunicabilityTypes.size();
}

PromotionLineAfflictionModifier CvTerrainInfo::getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile, bool bVicinity, bool bAccessVolume)
{
	FAssertMsg(iPromotionLine < (int)m_aAfflictionCommunicabilityTypes.size(), "Index out of bounds");
	FAssertMsg(iPromotionLine > -1, "Index out of bounds");
	if ((bWorkedTile && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bWorkedTile) ||
		(bVicinity && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bVicinity) ||
		(bAccessVolume && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bAccessVolume))
	{
		PromotionLineAfflictionModifier kMod = m_aAfflictionCommunicabilityTypes[iPromotionLine];
		kMod.iModifier = 0;
		return kMod;
	}

	return m_aAfflictionCommunicabilityTypes[iPromotionLine];
}

bool CvTerrainInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName( szTextVal, L"ArtDefineTag");
	setArtDefineTag(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"Yields"))
	{
		pXML->SetYields(&m_piYields);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piYields);

	if (pXML->TryMoveToXmlFirstChild(L"RiverYieldChange"))
	{
		pXML->SetYields(&m_piRiverYieldChange);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piRiverYieldChange);

	if (pXML->TryMoveToXmlFirstChild(L"HillsYieldChange"))
	{
		pXML->SetYields(&m_piHillsYieldChange);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piHillsYieldChange);

	pXML->GetOptionalChildXmlValByName(&m_bWater, L"bWater");
	pXML->GetOptionalChildXmlValByName(&m_bImpassable, L"bImpassable");
	pXML->GetOptionalChildXmlValByName(&m_bFound, L"bFound");
	pXML->GetOptionalChildXmlValByName(&m_bFoundCoast, L"bFoundCoast");
	pXML->GetOptionalChildXmlValByName(&m_bFoundFreshWater, L"bFoundFreshWater");

	pXML->GetOptionalChildXmlValByName(&m_iMovementCost, L"iMovement");
	pXML->GetOptionalChildXmlValByName(&m_iSeeFromLevel, L"iSeeFrom");
	pXML->GetOptionalChildXmlValByName(&m_iSeeThroughLevel, L"iSeeThrough");
	pXML->GetOptionalChildXmlValByName(&m_iBuildModifier, L"iBuildModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseModifier, L"iDefense");

	pXML->SetVariableListTagPairForAudioScripts(&m_pi3DAudioScriptFootstepIndex, L"FootstepSounds", GC.getFootstepAudioTypes(), GC.getNumFootstepAudioTypes());

	pXML->SetOptionalIntVector(&m_aiMapCategoryTypes, L"MapCategoryTypes");

	if (pXML->GetOptionalChildXmlValByName(szTextVal, L"WorldSoundscapeAudioScript"))
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	else
		m_iWorldSoundscapeScriptId = -1;
/************************************************************************************************/
/* Afforess					  Start		 04/21/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iCultureDistance, L"iCultureDistance");
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	m_PropertyManipulators.read(pXML);
	//TB Combat Mods begin
	pXML->GetOptionalChildXmlValByName(&m_bColdDamage, L"bColdDamage");
	//TB Combat Mods end
	if(pXML->TryMoveToXmlFirstChild(L"AfflictionCommunicabilityTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AfflictionCommunicabilityType" );
		m_aAfflictionCommunicabilityTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionCommunicabilityType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictionCommunicabilityTypes[i].ePromotionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].iModifier), L"iCommunicability");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bWorkedTile), L"bWorkedTile");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bVicinity), L"bVicinity");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bAccessVolume), L"bAccessVolume");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PromotionLineType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvTerrainInfo::copyNonDefaults(CvTerrainInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getArtDefineTag() == cDefault) setArtDefineTag(pClassInfo->getArtDefineTag());

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (!m_piYields)
		{
			if (pClassInfo->getYield(i))
			{
				pXML->InitList(&m_piYields, NUM_YIELD_TYPES);
			}
		}

		if (m_piYields && (m_piYields[i] == iDefault))
		{
			m_piYields[i] = pClassInfo->getYield(i);
		}

		if (!m_piRiverYieldChange)
		{
			if (pClassInfo->getRiverYieldChange(i))
			{
				pXML->InitList(&m_piRiverYieldChange, NUM_YIELD_TYPES);
			}
		}

		if (m_piRiverYieldChange && (m_piRiverYieldChange[i] == iDefault))
		{
			m_piRiverYieldChange[i] = pClassInfo->getRiverYieldChange(i);
		}

		if (!m_piHillsYieldChange)
		{
			if (pClassInfo->getHillsYieldChange(i))
			{
				pXML->InitList(&m_piHillsYieldChange, NUM_YIELD_TYPES);
			}
		}

		if (m_piHillsYieldChange && (m_piHillsYieldChange[i] == iDefault))
		{
			m_piHillsYieldChange[i] = pClassInfo->getHillsYieldChange(i);
		}
	}
	if (isWater() == bDefault) m_bWater = pClassInfo->isWater();
	if (isImpassable() == bDefault) m_bImpassable = pClassInfo->isImpassable();
	if (isFound() == bDefault) m_bFound = pClassInfo->isFound();
	if (isFoundCoast() == bDefault) m_bFoundCoast = pClassInfo->isFoundCoast();
	if (isFoundFreshWater() == bDefault) m_bFoundFreshWater = pClassInfo->isFoundFreshWater();
	if (getMovementCost() == iDefault) m_iMovementCost = pClassInfo->getMovementCost();
	if (getSeeFromLevel() == iDefault) m_iSeeFromLevel = pClassInfo->getSeeFromLevel();
	if (getSeeThroughLevel() == iDefault) m_iSeeThroughLevel = pClassInfo->getSeeThroughLevel();
	if (getBuildModifier() == iDefault) m_iBuildModifier = pClassInfo->getBuildModifier();
	if (getDefenseModifier() == iDefault) m_iDefenseModifier = pClassInfo->getDefenseModifier();

	for ( int i = 0; i < GC.getNumFootstepAudioTypes(); i++)
	{
		if (get3DAudioScriptFootstepIndex(i) == -1 && pClassInfo->get3DAudioScriptFootstepIndex(i) != -1)
		{
			if ( NULL == m_pi3DAudioScriptFootstepIndex )
			{
				CvXMLLoadUtility::InitList(&m_pi3DAudioScriptFootstepIndex,GC.getNumFootstepAudioTypes(),-1);
			}
			m_pi3DAudioScriptFootstepIndex[i] = pClassInfo->get3DAudioScriptFootstepIndex(i);
		}
	}

	if (getWorldSoundscapeScriptId() == iTextDefault) m_iWorldSoundscapeScriptId = pClassInfo->getWorldSoundscapeScriptId();
/************************************************************************************************/
/* Afforess					  Start		 04/21/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getCultureDistance() == iDefault) m_iCultureDistance = pClassInfo->getCultureDistance();
	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	if (getNumMapCategoryTypes() == 0)
	{
		m_aiMapCategoryTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumMapCategoryTypes(); i++)
		{
			m_aiMapCategoryTypes.push_back(pClassInfo->getMapCategoryType(i));
		}
	}
	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);

	//TB Combat Mods begin
	if (isColdDamage() == bDefault) m_bColdDamage = pClassInfo->isColdDamage();
	//TB Combat Mods end
}


void CvTerrainInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iMovementCost);
	CheckSum(iSum, m_iSeeFromLevel);
	CheckSum(iSum, m_iSeeThroughLevel);
	CheckSum(iSum, m_iBuildModifier);
	CheckSum(iSum, m_iDefenseModifier);

	CheckSum(iSum, m_bWater);
	CheckSum(iSum, m_bImpassable);
	CheckSum(iSum, m_bFound);
	CheckSum(iSum, m_bFoundCoast);
	CheckSum(iSum, m_bFoundFreshWater);

	// Arrays

	CheckSum(iSum, m_piYields, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piRiverYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piHillsYieldChange, NUM_YIELD_TYPES);

	CheckSum(iSum, m_iCultureDistance);
	CheckSum(iSum, m_iHealthPercent);

	m_PropertyManipulators.getCheckSum(iSum);
	//TB Combat Mods begin
	CheckSum(iSum, m_bColdDamage);
	CheckSumC(iSum, m_aiMapCategoryTypes);

	int iNumElements = m_aAfflictionCommunicabilityTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].iModifier);
	}
	//TB Combat Mods end
}

const TCHAR* CvTerrainInfo::getButton() const
{
/************************************************************************************************/
/* XMLCOPY								 10/25/07								MRGENIE	  */
/*																							  */
/* Catch non-existing tag																	   */
/************************************************************************************************/
	CvString cDefault = CvString::format("").GetCString();
	if (getArtDefineTag() == cDefault)
	{
		return NULL;
	}
	const CvArtInfoTerrain * pTerrainArtInfo;
	pTerrainArtInfo = getArtInfo();
	if (pTerrainArtInfo != NULL)
	{
		return pTerrainArtInfo->getButton();
	}
	else
	{
		return NULL;
	}
}

const CvArtInfoTerrain* CvTerrainInfo::getArtInfo() const
{
	return ARTFILEMGR.getTerrainArtInfo( getArtDefineTag());
}

CvPropertyManipulators* CvTerrainInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMapCategoryInfo
//
//  DESC:   Contains info about Map Categories
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvMapCategoryInfo::CvMapCategoryInfo() :
m_bInitialized(true)
{
}

CvMapCategoryInfo::~CvMapCategoryInfo()
{
}

bool CvMapCategoryInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	//Uncomment when needed
	//CvString szTextVal;

	return true;
}

bool CvMapCategoryInfo::readPass2(CvXMLLoadUtility* pXML)
{
	//Uncomment when needed
	//CvString szTextVal;


	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	return true;
}

void CvMapCategoryInfo::copyNonDefaults(CvMapCategoryInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	//Uncomment all of the following as needed
	//bool bDefault = false;
	//int iDefault = 0;
	//int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	//int iAudioDefault = -1;  //all audio is default -1
	//float fDefault = 0.0f;
	//CvString cDefault = CvString::format("").GetCString();

	//CvInfoBase::copyNonDefaults(pClassInfo, pXML);

}

void CvMapCategoryInfo::getCheckSum(unsigned int& iSum)
{
}

