//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertyInteraction.cpp
//
//  PURPOSE: Interaction of generic properties for Civ4 classes
//
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvPropertyInteraction::CvPropertyInteraction() :
									m_eSourceProperty(NO_PROPERTY),
									m_eTargetProperty(NO_PROPERTY),
									m_eObjectType(NO_GAMEOBJECT),
									m_eRelation(NO_RELATION),
									m_iRelationData(0),
									m_pExprActive(NULL)
{
}

CvPropertyInteraction::CvPropertyInteraction(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty) :
									m_eSourceProperty(eSourceProperty),
									m_eTargetProperty(eTargetProperty),
									m_eObjectType(NO_GAMEOBJECT),
									m_eRelation(NO_RELATION),
									m_iRelationData(0),
									m_pExprActive(NULL)
{
}

CvPropertyInteraction::~CvPropertyInteraction()
{
	GC.removeDelayedResolution((int*)&m_eSourceProperty);
	GC.removeDelayedResolution((int*)&m_eTargetProperty);
	SAFE_DELETE(m_pExprActive);
}

PropertyTypes CvPropertyInteraction::getSourceProperty() const
{
	return m_eSourceProperty;
}

PropertyTypes CvPropertyInteraction::getTargetProperty() const
{
	return m_eTargetProperty;
}

GameObjectTypes CvPropertyInteraction::getObjectType() const
{
	return m_eObjectType;
}

RelationTypes CvPropertyInteraction::getRelation() const
{
	return m_eRelation;
}

int CvPropertyInteraction::getRelationData() const
{
	return m_iRelationData;
}

bool CvPropertyInteraction::isActive(const CvGameObject *pObject) const
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

bool CvPropertyInteraction::read(CvXMLLoadUtility *pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, L"SourcePropertyType");
	//m_eSourceProperty = (PropertyTypes) pXML->FindInInfoClass(szTextVal);
	GC.addDelayedResolution((int*)&m_eSourceProperty,szTextVal);
	pXML->GetChildXmlValByName(szTextVal, L"TargetPropertyType");
	//m_eTargetProperty = (PropertyTypes) pXML->FindInInfoClass(szTextVal);
	GC.addDelayedResolution((int*)&m_eTargetProperty,szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"GameObjectType");
	m_eObjectType = (GameObjectTypes) pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"RelationType");
	m_eRelation = (RelationTypes) pXML->GetInfoClass(szTextVal);
	if (m_eRelation == RELATION_NEAR)
		pXML->GetOptionalChildXmlValByName(&m_iRelationData, L"iDistance");
	if (pXML->TryMoveToXmlFirstChild(L"Active"))
	{
		m_pExprActive = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	return true;
}

void CvPropertyInteraction::copyNonDefaults(CvPropertyInteraction* pProp)
{
//	if (m_eSourceProperty == NO_PROPERTY)
//		m_eSourceProperty = pProp->getSourceProperty();
	GC.copyNonDefaultDelayedResolution((int*)&m_eSourceProperty,(int*)&(pProp->m_eSourceProperty));
//	if (m_eTargetProperty == NO_PROPERTY)
//		m_eTargetProperty = pProp->getTargetProperty();
	GC.copyNonDefaultDelayedResolution((int*)&m_eTargetProperty,(int*)&(pProp->m_eTargetProperty));
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

void CvPropertyInteraction::buildDisplayString(CvWStringBuffer &szBuffer) const
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

void CvPropertyInteraction::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_eSourceProperty);
	CheckSum(iSum, m_eTargetProperty);
}



CvPropertyInteractionConvertConstant::CvPropertyInteractionConvertConstant() :	CvPropertyInteraction(),
																				m_iAmountPerTurn(0)
{
}

CvPropertyInteractionConvertConstant::CvPropertyInteractionConvertConstant(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty) : CvPropertyInteraction(eSourceProperty, eTargetProperty),
																																		m_iAmountPerTurn(0)
{
}

CvPropertyInteractionConvertConstant::CvPropertyInteractionConvertConstant(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty, int iAmountPerTurn) : CvPropertyInteraction(eSourceProperty, eTargetProperty), m_iAmountPerTurn(iAmountPerTurn)
{
}

PropertyInteractionTypes CvPropertyInteractionConvertConstant::getType() const
{
	return PROPERTYINTERACTION_CONVERT_CONSTANT;
}

int CvPropertyInteractionConvertConstant::getAmountPerTurn() const
{
	return m_iAmountPerTurn;
}

std::pair<int,int> CvPropertyInteractionConvertConstant::getPredict(int iCurrentAmountSource, int iCurrentAmountTarget) const
{
	return std::pair<int,int>(-m_iAmountPerTurn,m_iAmountPerTurn);
}

std::pair<int,int> CvPropertyInteractionConvertConstant::getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget) const
{
	return std::pair<int,int>(-m_iAmountPerTurn,m_iAmountPerTurn);
}

void CvPropertyInteractionConvertConstant::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTemp;
	szTemp.Format(L"Convert %+d %c to %c / Turn", m_iAmountPerTurn, GC.getPropertyInfo(getSourceProperty()).getChar(), GC.getPropertyInfo(getTargetProperty()).getChar());
	szBuffer.append(szTemp);
	CvPropertyInteraction::buildDisplayString(szBuffer);
}

bool CvPropertyInteractionConvertConstant::read(CvXMLLoadUtility *pXML)
{
	CvPropertyInteraction::read(pXML);
	pXML->GetOptionalChildXmlValByName(&m_iAmountPerTurn, L"iAmountPerTurn");
	return true;
}

void CvPropertyInteractionConvertConstant::copyNonDefaults(CvPropertyInteraction* pProp)
{
	CvPropertyInteraction::copyNonDefaults(pProp);
	const CvPropertyInteractionConvertConstant* pOther = static_cast<const CvPropertyInteractionConvertConstant*>(pProp);
	if (m_iAmountPerTurn == 0)
		m_iAmountPerTurn = pOther->getAmountPerTurn();
}

void CvPropertyInteractionConvertConstant::getCheckSum(uint32_t& iSum) const
{
	CvPropertyInteraction::getCheckSum(iSum);
	CheckSum(iSum, m_iAmountPerTurn);
}



CvPropertyInteractionInhibitedGrowth::CvPropertyInteractionInhibitedGrowth() :	CvPropertyInteraction(),
																				m_iGrowthPercent(0),
																				m_iInhibitionPercent(0)
{
}

CvPropertyInteractionInhibitedGrowth::CvPropertyInteractionInhibitedGrowth(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty) : CvPropertyInteraction(eSourceProperty, eTargetProperty),
																				m_iGrowthPercent(0),
																				m_iInhibitionPercent(0)
{
}

CvPropertyInteractionInhibitedGrowth::CvPropertyInteractionInhibitedGrowth(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty, int iGrowthPercent, int iInhibitionPercent) : CvPropertyInteraction(eSourceProperty, eTargetProperty),
																				m_iGrowthPercent(iGrowthPercent),
																				m_iInhibitionPercent(iInhibitionPercent)
{
}

PropertyInteractionTypes CvPropertyInteractionInhibitedGrowth::getType() const
{
	return PROPERTYINTERACTION_INHIBITED_GROWTH;
}

int CvPropertyInteractionInhibitedGrowth::getGrowthPercent() const
{
	return m_iGrowthPercent;
}

int CvPropertyInteractionInhibitedGrowth::getInhibitionPercent() const
{
	return m_iInhibitionPercent;
}

std::pair<int,int> CvPropertyInteractionInhibitedGrowth::getPredict(int iCurrentAmountSource, int iCurrentAmountTarget) const
{
	return std::pair<int,int>(std::max(0, iCurrentAmountSource * m_iGrowthPercent - iCurrentAmountTarget * m_iInhibitionPercent),0);
}

std::pair<int,int> CvPropertyInteractionInhibitedGrowth::getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget) const
{
	return std::pair<int,int>(std::max(0, iCurrentAmountSource * m_iGrowthPercent - iCurrentAmountTarget * m_iInhibitionPercent),0);
}

void CvPropertyInteractionInhibitedGrowth::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTemp;
	szTemp.Format(L"Growth of %+d %c / Turn inhibited by %d %c", m_iGrowthPercent, GC.getPropertyInfo(getSourceProperty()).getChar(), m_iInhibitionPercent, GC.getPropertyInfo(getTargetProperty()).getChar());
	szBuffer.append(szTemp);
	CvPropertyInteraction::buildDisplayString(szBuffer);
}

bool CvPropertyInteractionInhibitedGrowth::read(CvXMLLoadUtility *pXML)
{
	CvPropertyInteraction::read(pXML);
	pXML->GetOptionalChildXmlValByName(&m_iGrowthPercent, L"iGrowthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iInhibitionPercent, L"iInhibitionPercent");
	return true;
}

void CvPropertyInteractionInhibitedGrowth::copyNonDefaults(CvPropertyInteraction* pProp)
{
	CvPropertyInteraction::copyNonDefaults(pProp);
	const CvPropertyInteractionInhibitedGrowth* pOther = static_cast<const CvPropertyInteractionInhibitedGrowth*>(pProp);
	if (m_iGrowthPercent == 0)
		m_iGrowthPercent = pOther->getGrowthPercent();
	if (m_iInhibitionPercent == 0)
		m_iInhibitionPercent = pOther->getInhibitionPercent();
}

void CvPropertyInteractionInhibitedGrowth::getCheckSum(uint32_t& iSum) const
{
	CvPropertyInteraction::getCheckSum(iSum);
	CheckSum(iSum, m_iGrowthPercent);
	CheckSum(iSum, m_iInhibitionPercent);
}



CvPropertyInteractionConvertPercent::CvPropertyInteractionConvertPercent() :	CvPropertyInteraction(),
																				m_iPercent(0), m_iNoConvertAmount(0)
{
}

CvPropertyInteractionConvertPercent::CvPropertyInteractionConvertPercent(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty) : CvPropertyInteraction(eSourceProperty, eTargetProperty),
																																		m_iPercent(0), m_iNoConvertAmount(0)
{
}

CvPropertyInteractionConvertPercent::CvPropertyInteractionConvertPercent(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty, int iPercent, int iNoConvertAmount) : CvPropertyInteraction(eSourceProperty, eTargetProperty), m_iPercent(iPercent), m_iNoConvertAmount(iNoConvertAmount)
{
}

PropertyInteractionTypes CvPropertyInteractionConvertPercent::getType() const
{
	return PROPERTYINTERACTION_CONVERT_PERCENT;
}

int CvPropertyInteractionConvertPercent::getPercent() const
{
	return m_iPercent;
}

int CvPropertyInteractionConvertPercent::getNoConvertAmount() const
{
	return m_iNoConvertAmount;
}

std::pair<int,int> CvPropertyInteractionConvertPercent::getPredict(int iCurrentAmountSource, int iCurrentAmountTarget) const
{
	const int iAmount = std::max((iCurrentAmountSource - m_iNoConvertAmount) * m_iPercent / 100, 0);
	return std::pair<int,int>(-iAmount,iAmount);
}

std::pair<int,int> CvPropertyInteractionConvertPercent::getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget) const
{
	const int iAmount = std::max((iCurrentAmountSource - m_iNoConvertAmount) * m_iPercent / 100, 0);
	const int iExtra = iPredictedAmountSource - iAmount - iCurrentAmountSource;
	return getPredict(iCurrentAmountSource + iExtra/2, iCurrentAmountTarget);
}

void CvPropertyInteractionConvertPercent::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTemp;
	szTemp.Format(L"Convert %+d% %c to %c / Turn", m_iPercent, GC.getPropertyInfo(getSourceProperty()).getChar(), GC.getPropertyInfo(getTargetProperty()).getChar());
	szBuffer.append(szTemp);
	CvPropertyInteraction::buildDisplayString(szBuffer);
}

bool CvPropertyInteractionConvertPercent::read(CvXMLLoadUtility *pXML)
{
	CvPropertyInteraction::read(pXML);
	pXML->GetOptionalChildXmlValByName(&m_iPercent, L"iPercent");
	pXML->GetOptionalChildXmlValByName(&m_iNoConvertAmount, L"iNoConvertAmount");
	return true;
}

void CvPropertyInteractionConvertPercent::copyNonDefaults(CvPropertyInteraction* pProp)
{
	CvPropertyInteraction::copyNonDefaults(pProp);
	const CvPropertyInteractionConvertPercent* pOther = static_cast<const CvPropertyInteractionConvertPercent*>(pProp);
	if (m_iPercent == 0)
		m_iPercent = pOther->getPercent();
	if (m_iNoConvertAmount == 0)
		m_iNoConvertAmount = pOther->getNoConvertAmount();
}

void CvPropertyInteractionConvertPercent::getCheckSum(uint32_t& iSum) const
{
	CvPropertyInteraction::getCheckSum(iSum);
	CheckSum(iSum, m_iPercent);
	CheckSum(iSum, m_iNoConvertAmount);
}
