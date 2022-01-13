//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertySource.cpp
//
//  PURPOSE: Source of generic properties for Civ4 classes
//
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvGameTextMgr.h"
#include "CvGameObject.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"
#include "CvPropertySource.h"
#include "IntExpr.h"

CvPropertySource::CvPropertySource()
	: m_eProperty(NO_PROPERTY)
	, m_eObjectType(NO_GAMEOBJECT)
	, m_eRelation(NO_RELATION)
	, m_iRelationData(0)
	, m_pExprActive(NULL)
{
}

CvPropertySource::CvPropertySource(PropertyTypes eProperty)
	: m_eProperty(eProperty)
	, m_eObjectType(NO_GAMEOBJECT)
	, m_eRelation(NO_RELATION)
	, m_iRelationData(0)
	, m_pExprActive(NULL)
{
}

CvPropertySource::~CvPropertySource()
{
	GC.removeDelayedResolution((int*)&m_eProperty);
	SAFE_DELETE(m_pExprActive);
}

PropertyTypes CvPropertySource::getProperty() const
{
	return m_eProperty;
}

GameObjectTypes CvPropertySource::getObjectType() const
{
	return m_eObjectType;
}

RelationTypes CvPropertySource::getRelation() const
{
	return m_eRelation;
}

int CvPropertySource::getRelationData() const
{
	return m_iRelationData;
}

bool CvPropertySource::isActive(const CvGameObject* pObject) const
{
	if ((m_eObjectType == NO_GAMEOBJECT) || (m_eObjectType == pObject->getGameObjectType()))
	{
		if (m_pExprActive)
		{
			return m_pExprActive->evaluate(pObject);
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool CvPropertySource::read(CvXMLLoadUtility *pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, L"PropertyType");
	GC.addDelayedResolution((int*)&m_eProperty, szTextVal);
	//m_eProperty = (PropertyTypes) pXML->FindInInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"GameObjectType");
	m_eObjectType = (GameObjectTypes) pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"RelationType");
	m_eRelation = (RelationTypes) pXML->GetInfoClass(szTextVal);
	if (m_eRelation == RELATION_NEAR)
		pXML->GetChildXmlValByName(&m_iRelationData, L"iDistance");
	if (pXML->TryMoveToXmlFirstChild(L"Active"))
	{
		m_pExprActive = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	return true;
}

void CvPropertySource::copyNonDefaults(CvPropertySource* pProp)
{
	//if (m_eProperty == NO_PROPERTY)
	//	m_eProperty = pProp->getProperty();
	GC.copyNonDefaultDelayedResolution((int*)&m_eProperty, (int*)&(pProp->m_eProperty));

	if (m_eObjectType == NO_GAMEOBJECT)
		m_eObjectType = pProp->getObjectType();
	if (m_eRelation == NO_RELATION)
		m_eRelation = pProp->getRelation();
	if (m_iRelationData == 0)
		m_iRelationData = pProp->getRelationData();
	if (m_pExprActive == NULL)
	{
		m_pExprActive = pProp->m_pExprActive;
		pProp->m_pExprActive = NULL;
	}
}

void CvPropertySource::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	if ((m_eRelation != NO_RELATION) || (m_pExprActive))
	{
		szBuffer.append(" (");
		if (m_eRelation != NO_RELATION)
		{
			GAMETEXT.buildGameObjectRelationString(szBuffer, m_eObjectType, m_eRelation, m_iRelationData);
		}
		if (m_pExprActive)
		{
			if (m_eRelation != NO_RELATION)
				szBuffer.append(", ");
			szBuffer.append(gDLL->getText("TXT_KEY_PROPERTY_ACTIVE"));
			m_pExprActive->buildDisplayString(szBuffer);
		}
		szBuffer.append(")");
	}
}

void CvPropertySource::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_eProperty);
}


CvPropertySourceConstant::CvPropertySourceConstant()
	: CvPropertySource()
	, m_pAmountPerTurn(NULL)
{
}

CvPropertySourceConstant::CvPropertySourceConstant(PropertyTypes eProperty)
	: CvPropertySource(eProperty)
	, m_pAmountPerTurn(NULL)
{
}

CvPropertySourceConstant::CvPropertySourceConstant(PropertyTypes eProperty, const IntExpr* pAmountPerTurn)
	: CvPropertySource(eProperty)
	, m_pAmountPerTurn(pAmountPerTurn)
{
}

int CvPropertySourceConstant::getAmountPerTurn(const CvGameObject* pObject) const
{
	return m_pAmountPerTurn->evaluate(pObject);
}

int CvPropertySourceConstant::getSourcePredict(const CvGameObject* pObject, int iCurrentAmount, PropertySourceContext* pContext) const
{
	// We store the expression result in the property source context to make sure it stays constant in between prediction and correction
	const int iAmountPerTurn = getAmountPerTurn(pObject);
	if (pContext)
		pContext->setData1(iAmountPerTurn);
	return iAmountPerTurn;
}

int CvPropertySourceConstant::getSourceCorrect(const CvGameObject* pObject, int iCurrentAmount, int iPredictedAmount, const PropertySourceContext* pContext) const
{
	int iAmountPerTurn;
	if (pContext)
		iAmountPerTurn = pContext->getData1();
	else
		iAmountPerTurn = getAmountPerTurn(pObject);
	return iAmountPerTurn;
}

void CvPropertySourceConstant::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	m_pAmountPerTurn->buildDisplayString(szBuffer);
	szBuffer.append(" ");
	szBuffer.append(GC.getPropertyInfo(getProperty()).getChar());
	szBuffer.append(" ");
	szBuffer.append(gDLL->getText("TXT_KEY_PROPERTY_PER_TURN"));
	CvPropertySource::buildDisplayString(szBuffer);
}

bool CvPropertySourceConstant::read(CvXMLLoadUtility *pXML)
{
	OutputDebugString("Reading PropertySourceConstant");
	CvPropertySource::read(pXML);
	if (pXML->TryMoveToXmlFirstChild(L"iAmountPerTurn"))
	{
		m_pAmountPerTurn = IntExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	return true;
}

void CvPropertySourceConstant::copyNonDefaults(CvPropertySource* pProp)
{
	CvPropertySource::copyNonDefaults(pProp);
	if (m_pAmountPerTurn == NULL)
	{
		CvPropertySourceConstant* pOther = static_cast<CvPropertySourceConstant*>(pProp);
		m_pAmountPerTurn = pOther->m_pAmountPerTurn;
		pOther->m_pAmountPerTurn = NULL;
	}
}

void CvPropertySourceConstant::getCheckSum(uint32_t& iSum) const
{
	CvPropertySource::getCheckSum(iSum);
	m_pAmountPerTurn->getCheckSum(iSum);
}


CvPropertySourceConstantLimited::CvPropertySourceConstantLimited()
	: CvPropertySource()
	, m_iAmountPerTurn(0)
	, m_iLimit(0)
{
}

CvPropertySourceConstantLimited::CvPropertySourceConstantLimited(PropertyTypes eProperty)
	: CvPropertySource(eProperty)
	, m_iAmountPerTurn(0)
	, m_iLimit(0)
{
}

CvPropertySourceConstantLimited::CvPropertySourceConstantLimited(PropertyTypes eProperty, int iAmountPerTurn, int iLimit)
	: CvPropertySource(eProperty)
	, m_iAmountPerTurn(iAmountPerTurn)
	, m_iLimit(iLimit)
{
}

int CvPropertySourceConstantLimited::getSourcePredict(const CvGameObject* pObject, int iCurrentAmount, PropertySourceContext* pContext) const
{
	return m_iAmountPerTurn + iCurrentAmount > m_iLimit  ?  std::max(m_iLimit - iCurrentAmount, 0)  :  m_iAmountPerTurn;
}

int CvPropertySourceConstantLimited::getSourceCorrect(const CvGameObject* pObject, int iCurrentAmount, int iPredictedAmount, const PropertySourceContext* pContext) const
{
	if (iCurrentAmount >= m_iLimit)
	{
		return 0;
	}

	const int iPredict = getSourcePredict(pObject, iCurrentAmount);
	if (iPredictedAmount <= m_iLimit)
	{
		return iPredict;
	}
	else
	{
		const int iTotalPredicted = iPredictedAmount - iCurrentAmount;
		return (iPredict * (m_iLimit - iCurrentAmount)) / iTotalPredicted;
	}
}

void CvPropertySourceConstantLimited::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTemp;
	szTemp.Format(L"%c: %+d %s %d)", GC.getPropertyInfo(getProperty()).getChar(), m_iAmountPerTurn, gDLL->getText("TXT_KEY_PROPERTY_PER_TURN_LIMIT").c_str(), m_iLimit);
	szBuffer.append(szTemp);
	CvPropertySource::buildDisplayString(szBuffer);
}

bool CvPropertySourceConstantLimited::read(CvXMLLoadUtility *pXML)
{
	OutputDebugString("Reading PropertySourceConstantLimit");
	CvPropertySource::read(pXML);
	pXML->GetOptionalChildXmlValByName(&m_iAmountPerTurn, L"iAmountPerTurn");
	pXML->GetOptionalChildXmlValByName(&m_iLimit, L"iLimit");
	return true;
}

void CvPropertySourceConstantLimited::copyNonDefaults(CvPropertySource* pProp)
{
	CvPropertySource::copyNonDefaults(pProp);
	const CvPropertySourceConstantLimited* pOther = static_cast<const CvPropertySourceConstantLimited*>(pProp);
	if (m_iAmountPerTurn == 0)
		m_iAmountPerTurn = pOther->getAmountPerTurn();
	if (m_iLimit == 0)
		m_iLimit = pOther->getLimit();
}

void CvPropertySourceConstantLimited::getCheckSum(uint32_t& iSum) const
{
	CvPropertySource::getCheckSum(iSum);
	CheckSum(iSum, m_iAmountPerTurn);
	CheckSum(iSum, m_iLimit);
}



CvPropertySourceDecay::CvPropertySourceDecay()
	: CvPropertySource()
	, m_iPercent(0)
	, m_iNoDecayAmount(0)
{
}

CvPropertySourceDecay::CvPropertySourceDecay(PropertyTypes eProperty)
	: CvPropertySource(eProperty)
	, m_iPercent(0)
	, m_iNoDecayAmount(0)
{
}

CvPropertySourceDecay::CvPropertySourceDecay(PropertyTypes eProperty, int iPercent, int iNoDecayAmount)
	: CvPropertySource(eProperty)
	, m_iPercent(iPercent)
	, m_iNoDecayAmount(iNoDecayAmount)
{
}

bool CvPropertySourceDecay::isActive(const CvGameObject* pObject) const
{
	int iVal = pObject->getProperties()->getValueByProperty(m_eProperty);
	iVal = iVal < 0 ? -iVal : iVal;
	if (iVal > m_iNoDecayAmount)
	{
		return CvPropertySource::isActive(pObject);
	}
	else
	{
		return false;
	}
}

int CvPropertySourceDecay::getSourcePredict(const CvGameObject* pObject, int iCurrentAmount, PropertySourceContext* pContext) const
{
	if (iCurrentAmount >= 0)
		return - (m_iPercent * std::max(iCurrentAmount - m_iNoDecayAmount, 0)) / 100;
	else
		return (m_iPercent * std::max(-iCurrentAmount - m_iNoDecayAmount, 0)) / 100;
}

int CvPropertySourceDecay::getSourceCorrect(const CvGameObject* pObject, int iCurrentAmount, int iPredictedAmount, const PropertySourceContext* pContext) const
{
	if (iCurrentAmount >= 0)
	{
		int iDiff = iCurrentAmount - m_iNoDecayAmount;
		if (iDiff <= 0)
		{
			return 0;
		}

		int iPredicted = iCurrentAmount - (iDiff * m_iPercent) / 100;
		int iExtra = iPredictedAmount - iPredicted;
		//use half of extra to base decay on
		iDiff += iExtra / 2;
		return - std::max(0, (iDiff * m_iPercent) / 100);
	}
	else
	{
		int iDiff = -iCurrentAmount - m_iNoDecayAmount;
		if (iDiff <= 0)
		{
			return 0;
		}

		int iPredicted = iCurrentAmount + (iDiff * m_iPercent) / 100;
		int iExtra = iPredictedAmount - iPredicted;
		//use half of extra to base decay on
		iDiff += iExtra / 2;
		return std::max(0, (iDiff * m_iPercent) / 100);
	}
}

void CvPropertySourceDecay::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTemp;
	szTemp.Format(L"%c: %+d%% %s %d)", GC.getPropertyInfo(getProperty()).getChar(), m_iPercent, gDLL->getText("TXT_KEY_PROPERTY_PER_TURN_DECAY").c_str(), m_iNoDecayAmount);
	szBuffer.append(szTemp);
	CvPropertySource::buildDisplayString(szBuffer);
}

bool CvPropertySourceDecay::read(CvXMLLoadUtility *pXML)
{
	OutputDebugString("Reading PropertySourceDecay");
	CvPropertySource::read(pXML);
	pXML->GetOptionalChildXmlValByName(&m_iPercent, L"iPercent");
	pXML->GetOptionalChildXmlValByName(&m_iNoDecayAmount, L"iNoDecayAmount");
	return true;
}

void CvPropertySourceDecay::copyNonDefaults(CvPropertySource* pProp)
{
	CvPropertySource::copyNonDefaults(pProp);
	const CvPropertySourceDecay* pOther = static_cast<const CvPropertySourceDecay*>(pProp);
	if (m_iPercent == 0)
		m_iPercent = pOther->getPercent();
	if (m_iNoDecayAmount == 0)
		m_iNoDecayAmount = pOther->getNoDecayAmount();
}

void CvPropertySourceDecay::getCheckSum(uint32_t& iSum) const
{
	CvPropertySource::getCheckSum(iSum);
	CheckSum(iSum, m_iPercent);
	CheckSum(iSum, m_iNoDecayAmount);
}


CvPropertySourceAttributeConstant::CvPropertySourceAttributeConstant()
	: CvPropertySource()
	, m_iAmountPerTurn(0)
	, m_eAttribute(NO_ATTRIBUTE)
{
}

CvPropertySourceAttributeConstant::CvPropertySourceAttributeConstant(PropertyTypes eProperty)
	: CvPropertySource(eProperty)
	, m_iAmountPerTurn(0)
	, m_eAttribute(NO_ATTRIBUTE)
{
}

CvPropertySourceAttributeConstant::CvPropertySourceAttributeConstant(PropertyTypes eProperty, AttributeTypes eAttribute, int iAmountPerTurn)
	: CvPropertySource(eProperty)
	, m_iAmountPerTurn(iAmountPerTurn)
	, m_eAttribute(eAttribute)
{
}

int CvPropertySourceAttributeConstant::getSourcePredict(const CvGameObject* pObject, int iCurrentAmount, PropertySourceContext* pContext) const
{
	return pObject->getAttribute(m_eAttribute) * m_iAmountPerTurn;
}

int CvPropertySourceAttributeConstant::getSourceCorrect(const CvGameObject* pObject, int iCurrentAmount, int iPredictedAmount, const PropertySourceContext* pContext) const
{
	return pObject->getAttribute(m_eAttribute) * m_iAmountPerTurn;
}

void CvPropertySourceAttributeConstant::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTemp;
	szTemp.Format(L"%c: %+d %s", GC.getPropertyInfo(getProperty()).getChar(), m_iAmountPerTurn, gDLL->getText("TXT_KEY_PROPERTY_PER_TURN").c_str()); // TODO: proper display of attribute
	szBuffer.append(szTemp);
	CvPropertySource::buildDisplayString(szBuffer);
}

bool CvPropertySourceAttributeConstant::read(CvXMLLoadUtility *pXML)
{
	OutputDebugString("Reading PropertySourceAtributeConstant");
	CvPropertySource::read(pXML);
	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"AttributeType", "NO_ATTRIBUTE");
	m_eAttribute = (AttributeTypes)pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iAmountPerTurn, L"iAmountPerTurn");
	return true;
}

void CvPropertySourceAttributeConstant::copyNonDefaults(CvPropertySource* pProp)
{
	CvPropertySource::copyNonDefaults(pProp);
	const CvPropertySourceAttributeConstant* pOther = static_cast<const CvPropertySourceAttributeConstant*>(pProp);
	if (m_eAttribute == NO_ATTRIBUTE)
		m_eAttribute = pOther->getAttribute();
	if (m_iAmountPerTurn == 0)
		m_iAmountPerTurn = pOther->getAmountPerTurn();
}

void CvPropertySourceAttributeConstant::getCheckSum(uint32_t& iSum) const
{
	CvPropertySource::getCheckSum(iSum);
	CheckSum(iSum, (int)m_eAttribute);
	CheckSum(iSum, m_iAmountPerTurn);
}
