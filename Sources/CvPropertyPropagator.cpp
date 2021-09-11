//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertyPropagator.cpp
//
//  PURPOSE: Propagator of generic properties for Civ4 classes
//
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvPropertyPropagator::CvPropertyPropagator() :
									m_eProperty(NO_PROPERTY),
									m_eObjectType(NO_GAMEOBJECT),
									m_eRelation(NO_RELATION),
									m_iRelationData(0),
									m_eTargetRelation(NO_RELATION),
									m_eTargetObjectType(NO_GAMEOBJECT),
									m_iTargetRelationData(0),
									m_pExprActive(NULL),
									m_pExprTargetCondition(NULL)
{
}

CvPropertyPropagator::CvPropertyPropagator(PropertyTypes eProperty) :
									m_eProperty(eProperty),
									m_eObjectType(NO_GAMEOBJECT),
									m_eRelation(NO_RELATION),
									m_iRelationData(0),
									m_eTargetRelation(NO_RELATION),
									m_eTargetObjectType(NO_GAMEOBJECT),
									m_iTargetRelationData(0),
									m_pExprActive(NULL),
									m_pExprTargetCondition(NULL)
{
}

CvPropertyPropagator::~CvPropertyPropagator()
{
	GC.removeDelayedResolution((int*)&m_eProperty);
	SAFE_DELETE(m_pExprActive);
}

PropertyTypes CvPropertyPropagator::getProperty() const
{
	return m_eProperty;
}

GameObjectTypes CvPropertyPropagator::getObjectType() const
{
	return m_eObjectType;
}

RelationTypes CvPropertyPropagator::getRelation() const
{
	return m_eRelation;
}

int CvPropertyPropagator::getRelationData() const
{
	return m_iRelationData;
}

RelationTypes CvPropertyPropagator::getTargetRelation() const
{
	return m_eTargetRelation;
}

int CvPropertyPropagator::getTargetRelationData() const
{
	return m_iTargetRelationData;
}

GameObjectTypes CvPropertyPropagator::getTargetObjectType() const
{
	return m_eTargetObjectType;
}

bool CvPropertyPropagator::isActive(const CvGameObject* pObject) const
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

void CvPropertyPropagator::getTargetObjects(const CvGameObject* pObject, std::vector<const CvGameObject*>& apGameObjects) const
{
	apGameObjects.push_back(pObject);
	if (m_eTargetObjectType != NO_GAMEOBJECT)
	{
		pObject->enumerateRelatedCond(apGameObjects, m_eTargetObjectType, m_eTargetRelation, m_pExprTargetCondition, m_iTargetRelationData);
	}
	else
	{
		for (int i=0; i<NUM_GAMEOBJECTS; i++)
		{
			pObject->enumerateRelatedCond(apGameObjects, (GameObjectTypes)i, m_eTargetRelation, m_pExprTargetCondition, m_iTargetRelationData);
		}
	}
	// TODO: Should still filter out the source object from the vector
}

bool CvPropertyPropagator::read(CvXMLLoadUtility *pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, L"PropertyType");
//	m_eProperty = (PropertyTypes) pXML->GetInfoClass(szTextVal);
	GC.addDelayedResolution((int*)&m_eProperty, szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"GameObjectType");
	m_eObjectType = (GameObjectTypes) pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"RelationType");
	m_eRelation = (RelationTypes) pXML->GetInfoClass(szTextVal);
	if (m_eRelation == RELATION_NEAR)
		pXML->GetOptionalChildXmlValByName(&m_iRelationData, L"iDistance");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"TargetObjectType");
	m_eTargetObjectType = (GameObjectTypes) pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"TargetRelationType");
	m_eTargetRelation = (RelationTypes) pXML->GetInfoClass(szTextVal);
	if (m_eTargetRelation == RELATION_NEAR)
		pXML->GetOptionalChildXmlValByName(&m_iTargetRelationData, L"iTargetDistance");
	if (pXML->TryMoveToXmlFirstChild(L"Active"))
	{
		m_pExprActive = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	if (pXML->TryMoveToXmlFirstChild(L"TargetCondition"))
	{
		m_pExprTargetCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	return true;
}

void CvPropertyPropagator::copyNonDefaults(CvPropertyPropagator* pProp)
{
//	if (m_eProperty == NO_PROPERTY)
//		m_eProperty = pProp->getProperty();
	GC.copyNonDefaultDelayedResolution((int*)&m_eProperty,(int*)&(pProp->m_eProperty));
	if (m_eObjectType == NO_GAMEOBJECT)
		m_eObjectType = pProp->getObjectType();
	if (m_eRelation == NO_RELATION)
		m_eRelation = pProp->getRelation();
	if (m_iRelationData == 0)
		m_iRelationData = pProp->getRelationData();
	if (m_eTargetObjectType == NO_GAMEOBJECT)
		m_eTargetObjectType = pProp->getTargetObjectType();
	if (m_eTargetRelation == NO_RELATION)
		m_eTargetRelation = pProp->getTargetRelation();
	if (m_iTargetRelationData == 0)
		m_iTargetRelationData = pProp->getTargetRelationData();
	if (m_pExprActive == NULL)
	{
		m_pExprActive = pProp->m_pExprActive;
		pProp->m_pExprActive = NULL;
	}
	if (m_pExprTargetCondition == NULL)
	{
		m_pExprTargetCondition = pProp->m_pExprTargetCondition;
		pProp->m_pExprTargetCondition = NULL;
	}
}

void CvPropertyPropagator::buildDisplayString(CvWStringBuffer &szBuffer) const
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

void CvPropertyPropagator::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_eProperty);
}



CvPropertyPropagatorSpread::CvPropertyPropagatorSpread() :	CvPropertyPropagator(),
															m_iPercent(0)
{
}

CvPropertyPropagatorSpread::CvPropertyPropagatorSpread(PropertyTypes eProperty) : CvPropertyPropagator(eProperty),
																				m_iPercent(0)
{
}

CvPropertyPropagatorSpread::CvPropertyPropagatorSpread(PropertyTypes eProperty, int iPercent) : CvPropertyPropagator(eProperty), m_iPercent(iPercent)
{
}

PropertyPropagatorTypes CvPropertyPropagatorSpread::getType() const
{
	return PROPERTYPROPAGATOR_SPREAD;
}

int CvPropertyPropagatorSpread::getPercent() const
{
	return m_iPercent;
}

//bool CvPropertyPropagatorSpread::isActive(CvGameObject *pObject)
//{
//	if (pObject->getProperties()->getValueByProperty(m_eProperty))
//	{
//		return CvPropertyPropagator::isActive(pObject);
//	}
//	else
//	{
//		return false;
//	}
//}

void CvPropertyPropagatorSpread::getPredict(const std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict) const
{
	const int iCurrentAmount = aiCurrentAmount[0];
	aiPredict[0] = 0;
	for(int iI=1; iI<(int)aiCurrentAmount.size(); iI++)
	{
		const int iDiff = iCurrentAmount - aiCurrentAmount[iI];
		aiPredict[iI] = std::max(0, (iDiff * m_iPercent) / 100);
	}
}

void CvPropertyPropagatorSpread::getCorrect(const std::vector<int>& aiCurrentAmount, const std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect) const
{
	const int iCurrentAmount = aiCurrentAmount[0];
	aiCorrect[0] = 0;
	for(int iI=1; iI<(int)aiCurrentAmount.size(); iI++)
	{
		int iDiff = iCurrentAmount - aiCurrentAmount[iI];
		if (iDiff < 0)
		{
			aiCorrect[iI] = 0;
		}
		else
		{
			const int iPredicted = aiCurrentAmount[iI] + (iDiff * m_iPercent) / 100;
			const int iExtra = aiPredictedAmount[iI] - iPredicted;
			if (iExtra > 0)
			{
				//use half of extra to base spreading on
				iDiff -= iExtra / 2;
				aiCorrect[iI] = std::max(0, (iDiff * m_iPercent) / 100);
			}
			else
			{
				aiCorrect[iI] =(iDiff * m_iPercent) / 100;
			}
		}
	}
}

void CvPropertyPropagatorSpread::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTemp;
	szTemp.Format(L"Spreads %d%% %c difference / Turn", m_iPercent, GC.getPropertyInfo(getProperty()).getChar());
	szBuffer.append(szTemp);
	CvPropertyPropagator::buildDisplayString(szBuffer);
}

bool CvPropertyPropagatorSpread::read(CvXMLLoadUtility *pXML)
{
	CvPropertyPropagator::read(pXML);
	pXML->GetOptionalChildXmlValByName(&m_iPercent, L"iPercent");
	return true;
}

void CvPropertyPropagatorSpread::copyNonDefaults(CvPropertyPropagator* pProp)
{
	CvPropertyPropagator::copyNonDefaults(pProp);
	const CvPropertyPropagatorSpread* pOther = static_cast<const CvPropertyPropagatorSpread*>(pProp);
	if (m_iPercent == 0)
		m_iPercent = pOther->getPercent();
}

void CvPropertyPropagatorSpread::getCheckSum(uint32_t& iSum) const
{
	CvPropertyPropagator::getCheckSum(iSum);
	CheckSum(iSum, m_iPercent);
}



CvPropertyPropagatorGather::CvPropertyPropagatorGather() :	CvPropertyPropagator(),
															m_iAmountPerTurn(0)
{
}

CvPropertyPropagatorGather::CvPropertyPropagatorGather(PropertyTypes eProperty) : CvPropertyPropagator(eProperty),
																				m_iAmountPerTurn(0)
{
}

CvPropertyPropagatorGather::CvPropertyPropagatorGather(PropertyTypes eProperty, int iAmountPerTurn) : CvPropertyPropagator(eProperty), m_iAmountPerTurn(iAmountPerTurn)
{
}

PropertyPropagatorTypes CvPropertyPropagatorGather::getType() const
{
	return PROPERTYPROPAGATOR_GATHER;
}

int CvPropertyPropagatorGather::getAmountPerTurn() const
{
	return m_iAmountPerTurn;
}

void CvPropertyPropagatorGather::getPredict(const std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict) const
{
	aiPredict[0] = 0;
	for(int iI=1; iI<(int)aiCurrentAmount.size(); iI++)
	{
		if (aiCurrentAmount[iI] < m_iAmountPerTurn)
		{
			aiPredict[0] += aiCurrentAmount[iI];
			aiPredict[iI] = -aiCurrentAmount[iI];
		}
		else
		{
			aiPredict[0] += m_iAmountPerTurn;
			aiPredict[iI] = -m_iAmountPerTurn;
		}
	}
}

void CvPropertyPropagatorGather::getCorrect(const std::vector<int>& aiCurrentAmount, const std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect) const
{
	aiCorrect[0] = 0;
	for(int iI=1; iI<(int)aiCurrentAmount.size(); iI++)
	{
		if (aiPredictedAmount[iI] >= 0)
		{
			if (aiCurrentAmount[iI] < m_iAmountPerTurn)
			{
				aiCorrect[0] += aiCurrentAmount[iI];
				aiCorrect[iI] = -aiCurrentAmount[iI];
			}
			else
			{
				aiCorrect[0] += m_iAmountPerTurn;
				aiCorrect[iI] = -m_iAmountPerTurn;
			}
		}
		else
		{
			const int iPredicted = (aiCurrentAmount[iI] < m_iAmountPerTurn) ? aiCurrentAmount[iI] : m_iAmountPerTurn;
			const int iCorrected = (iPredicted * aiCurrentAmount[iI]) / (aiCurrentAmount[iI] - aiPredictedAmount[iI]);
			aiCorrect[0] += iCorrected;
			aiCorrect[iI] = -iCorrected;
		}
	}
}

void CvPropertyPropagatorGather::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTemp;
	szTemp.Format(L"Gathers %d %c / Turn", m_iAmountPerTurn, GC.getPropertyInfo(getProperty()).getChar());
	szBuffer.append(szTemp);
	CvPropertyPropagator::buildDisplayString(szBuffer);
}

bool CvPropertyPropagatorGather::read(CvXMLLoadUtility *pXML)
{
	CvPropertyPropagator::read(pXML);
	pXML->GetOptionalChildXmlValByName(&m_iAmountPerTurn, L"iAmountPerTurn");
	return true;
}

void CvPropertyPropagatorGather::copyNonDefaults(CvPropertyPropagator* pProp)
{
	CvPropertyPropagator::copyNonDefaults(pProp);
	const CvPropertyPropagatorGather* pOther = static_cast<const CvPropertyPropagatorGather*>(pProp);
	if (m_iAmountPerTurn == 0)
		m_iAmountPerTurn = pOther->getAmountPerTurn();
}

void CvPropertyPropagatorGather::getCheckSum(uint32_t& iSum) const
{
	CvPropertyPropagator::getCheckSum(iSum);
	CheckSum(iSum, m_iAmountPerTurn);
}


CvPropertyPropagatorDiffuse::CvPropertyPropagatorDiffuse() :	CvPropertyPropagator(),
															m_iPercent(0)
{
}

CvPropertyPropagatorDiffuse::CvPropertyPropagatorDiffuse(PropertyTypes eProperty) : CvPropertyPropagator(eProperty),
																				m_iPercent(0)
{
}

CvPropertyPropagatorDiffuse::CvPropertyPropagatorDiffuse(PropertyTypes eProperty, int iPercent) : CvPropertyPropagator(eProperty), m_iPercent(iPercent)
{
}

PropertyPropagatorTypes CvPropertyPropagatorDiffuse::getType() const
{
	return PROPERTYPROPAGATOR_DIFFUSE;
}

int CvPropertyPropagatorDiffuse::getPercent() const
{
	return m_iPercent;
}

//bool CvPropertyPropagatorDiffuse::isActive(CvGameObject *pObject)
//{
//	if (pObject->getProperties()->getValueByProperty(m_eProperty))
//	{
//		return CvPropertyPropagator::isActive(pObject);
//	}
//	else
//	{
//		return false;
//	}
//}

void CvPropertyPropagatorDiffuse::getPredict(const std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict) const
{
	const int iCurrentAmount = aiCurrentAmount[0];
	aiPredict[0] = 0;
	for(int iI=1; iI<(int)aiCurrentAmount.size(); iI++)
	{
		const int iDiff = iCurrentAmount - aiCurrentAmount[iI];
		const int iChange = std::max(0, (iDiff * m_iPercent) / 100);
		aiPredict[iI] = iChange;
		aiPredict[0] -= iChange;
	}
}

void CvPropertyPropagatorDiffuse::getCorrect(const std::vector<int>& aiCurrentAmount, const std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect) const
{
	const int iCurrentAmount = aiCurrentAmount[0];
	const int iPredictedAmount = aiPredictedAmount[0];
	int iPredictedSelf = 0;
	const int iPredictedTotalSelf = iPredictedAmount - iCurrentAmount;
	aiCorrect[0] = 0;
	for(int iI=1; iI<(int)aiCurrentAmount.size(); iI++)
	{
		int iDiff = iCurrentAmount - aiCurrentAmount[iI];
		if (iDiff < 0)
		{
			aiCorrect[iI] = 0;
		}
		else
		{
			int iChange = (iDiff * m_iPercent) / 100;
			const int iPredicted = aiCurrentAmount[iI] + iChange;
			iPredictedSelf -= iChange;
			const int iExtra = aiPredictedAmount[iI] - iPredicted;
			if (iExtra > 0)
			{
				//use half of extra to base diffusion on
				iDiff -= iExtra / 2;
			}

			iChange = std::max(0, (iDiff * m_iPercent) / 100);
			aiCorrect[iI] = iChange;
			aiCorrect[0] -= iChange;
		}
	}

	if (iPredictedTotalSelf < iPredictedSelf)
	{
		const int iSelfChangeByOthers = iPredictedTotalSelf - iPredictedSelf;
		// use half of other change to base diffusion on
		const int iAssumedAmount = iCurrentAmount + iSelfChangeByOthers / 2;

		aiCorrect[0] = 0;

		for(int iI=1; iI<(int)aiCurrentAmount.size(); iI++)
		{
			int iDiff = iAssumedAmount - aiCurrentAmount[iI];
			if (iDiff < 0)
			{
				aiCorrect[iI] = 0;
			}
			else
			{
				int iChange = (iDiff * m_iPercent) / 100;
				const int iPredicted = aiCurrentAmount[iI] + iChange;
				const int iExtra = aiPredictedAmount[iI] - iPredicted;
				if (iExtra > 0)
				{
					//use half of extra to base diffusion on
					iDiff -= iExtra / 2;
				}

				iChange = std::max(0, (iDiff * m_iPercent) / 100);
				aiCorrect[iI] = iChange;
				aiCorrect[0] -= iChange;
			}
		}
	}
}

void CvPropertyPropagatorDiffuse::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTemp;
	szTemp.Format(L"%d%% %c diffusion / Turn", m_iPercent, GC.getPropertyInfo(getProperty()).getChar());
	szBuffer.append(szTemp);
	CvPropertyPropagator::buildDisplayString(szBuffer);
}

bool CvPropertyPropagatorDiffuse::read(CvXMLLoadUtility *pXML)
{
	CvPropertyPropagator::read(pXML);
	pXML->GetOptionalChildXmlValByName(&m_iPercent, L"iPercent");
	return true;
}

void CvPropertyPropagatorDiffuse::copyNonDefaults(CvPropertyPropagator* pProp)
{
	CvPropertyPropagator::copyNonDefaults(pProp);
	const CvPropertyPropagatorDiffuse* pOther = static_cast<const CvPropertyPropagatorDiffuse*>(pProp);
	if (m_iPercent == 0)
		m_iPercent = pOther->getPercent();
}

void CvPropertyPropagatorDiffuse::getCheckSum(uint32_t& iSum) const
{
	CvPropertyPropagator::getCheckSum(iSum);
	CheckSum(iSum, m_iPercent);
}
