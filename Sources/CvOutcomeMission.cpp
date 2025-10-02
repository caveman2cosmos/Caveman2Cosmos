//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvOutcomeMission.cpp
//
//  PURPOSE: A mission that has a cost and a result depending on an outcome list
//
//------------------------------------------------------------------------------------------------

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvGameObject.h"
#include "CvGlobals.h"
#include "CvOutcomeMission.h"
#include "CvPlayerAI.h"
#include "CvUnit.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"
#include "IntExpr.h"
#include "BoolExpr.h"

CvOutcomeMission::CvOutcomeMission() :
m_eMission(NO_MISSION),
m_iCost(NULL),
m_bKill(true),
m_ePayerType(NO_GAMEOBJECT),
m_pPlotCondition(NULL),
m_pUnitCondition(NULL)
{
}

CvOutcomeMission::~CvOutcomeMission()
{
	GC.removeDelayedResolution((int*)&m_eMission);
	SAFE_DELETE(m_iCost);
	SAFE_DELETE(m_pPlotCondition);
	SAFE_DELETE(m_pUnitCondition);
}

//const IntExpr* CvOutcomeMission::getCost() const
//{
//	return m_iCost;
//}

MissionTypes CvOutcomeMission::getMission() const
{
	return m_eMission;
}

const CvOutcomeList* CvOutcomeMission::getOutcomeList() const
{
	return &m_OutcomeList;
}

const CvProperties* CvOutcomeMission::getPropertyCost() const
{
	return &m_PropertyCost;
}

bool CvOutcomeMission::isKill() const
{
	return m_bKill;
}

GameObjectTypes CvOutcomeMission::getPayerType() const
{
	return m_ePayerType;
}

void callSetPayer(const CvGameObject* pObject, const CvGameObject** ppPayer)
{
	*ppPayer = pObject;
}

bool CvOutcomeMission::isPossible(const CvUnit* pUnit, bool bTestVisible) const
{
	PROFILE_EXTRA_FUNC();
	//if (!bTestVisible)
	//{
		if (m_iCost)
		{
			if (GET_PLAYER(pUnit->getOwner()).getGold() < m_iCost->evaluate(pUnit->getGameObject()))
			{
				return false;
			}
		}

		if (m_pPlotCondition)
			if (!m_pPlotCondition->evaluate(pUnit->plot()->getGameObject()))
				return false;
	//}

	if (m_pUnitCondition)
		if (!m_pUnitCondition->evaluate(pUnit->getGameObject()))
			return false;

	if (!getOutcomeList()->isPossible(*pUnit))
	{
		return false;
	}

	//if (!bTestVisible)
	//{
		if (!getPropertyCost()->isEmpty())
		{
			const CvGameObject* pPayer = NULL;
			if ((m_ePayerType == NO_GAMEOBJECT) || (m_ePayerType == GAMEOBJECT_UNIT))
			{
				pPayer = pUnit->getGameObject();
			}
			else
			{
				pUnit->getGameObject()->foreach(m_ePayerType, bind(callSetPayer, _1, &pPayer));
			}

			if (!pPayer)
			{
				return false;
			}

			if (! (*(pPayer->getProperties()) > m_PropertyCost ))
			{
				return false;
			}
		}
	//}

	return true;
}

void CvOutcomeMission::buildDisplayString(CvWStringBuffer& szBuffer, const CvUnit* pUnit) const
{
	if (!m_PropertyCost.isEmpty())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_COST"));
		m_PropertyCost.buildCompactChangesString(szBuffer);
	}

	if (m_iCost)
	{
		if (m_iCost->evaluate(pUnit->getGameObject()) != 0)
		{	/*GC.getGame().getGameObject()->adaptValueToGame(m_iID, m_pExpr->evaluate(GC.getGame().getGameObject())*/
			CvWString szTempBuffer;

			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_COST"));
			m_iCost->buildDisplayString(szBuffer);
		}
	}

	if (m_pPlotCondition)
	{
		if (!m_pPlotCondition->evaluate(pUnit->plot()->getGameObject()))
		{
			szBuffer.append(gDLL->getText("TXT_KEY_REQUIRES"));
			m_pPlotCondition->buildDisplayString(szBuffer);
		}
	}

	if (m_pUnitCondition)
	{
		if (!m_pUnitCondition->evaluate(pUnit->getGameObject()))
		{
			szBuffer.append(gDLL->getText("TXT_KEY_REQUIRES"));
			m_pUnitCondition->buildDisplayString(szBuffer);
		}
	}

	if (m_bKill)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_ACTION_CONSUME_UNIT"));
	}

	m_OutcomeList.buildDisplayString(szBuffer, *pUnit);
}

void CvOutcomeMission::execute(CvUnit* pUnit) const
{
	PROFILE_EXTRA_FUNC();
	if (m_iCost)
	{
		GET_PLAYER(pUnit->getOwner()).changeGold(-(m_iCost->evaluate(pUnit->getGameObject())));
	}

	m_OutcomeList.execute(*pUnit);

	if (!getPropertyCost()->isEmpty())
	{
		const CvGameObject* pPayer = NULL;
		if ((m_ePayerType == NO_GAMEOBJECT) || (m_ePayerType == GAMEOBJECT_UNIT))
		{
			pPayer = pUnit->getGameObject();
		}
		else
		{
			pUnit->getGameObject()->foreach(m_ePayerType, bind(callSetPayer, _1, &pPayer));
		}

		if (pPayer)
		{
			pPayer->getProperties()->subtractProperties(&m_PropertyCost);
		}
	}

	pUnit->finishMoves();
}

bool CvOutcomeMission::read(CvXMLLoadUtility *pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, L"MissionType");
	GC.addDelayedResolution((int*)&m_eMission, szTextVal);
	//m_eMission = (MissionTypes) pXML->FindInInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_bKill, L"bKill", true);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PayerType");
	m_ePayerType = (GameObjectTypes) pXML->GetInfoClass(szTextVal);
	m_PropertyCost.read(pXML, L"PropertyCost");
	m_OutcomeList.read(pXML, L"ActionOutcomes");
	if (pXML->TryMoveToXmlFirstChild(L"PlotCondition"))
	{
		m_pPlotCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"UnitCondition"))
	{
		m_pUnitCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"iCost"))
	{
		m_iCost = IntExpr::read(pXML);
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvOutcomeMission::copyNonDefaults(CvOutcomeMission* pOutcomeMission)
{
	GC.copyNonDefaultDelayedResolution((int*)&m_eMission, (int*)&(pOutcomeMission->m_eMission));
	//if (m_eMission == NO_MISSION)
	//{
	//	m_eMission = pOutcomeMission->getMission();
	//}

	if (m_bKill)
	{
		m_bKill = pOutcomeMission->isKill();
	}

	if (m_ePayerType == NO_GAMEOBJECT)
	{
		m_ePayerType = pOutcomeMission->getPayerType();
	}

	m_PropertyCost.copyNonDefaults(pOutcomeMission->getPropertyCost());
	m_OutcomeList.copyNonDefaults(&pOutcomeMission->m_OutcomeList);
	if (!m_pPlotCondition)
	{
		m_pPlotCondition = pOutcomeMission->m_pPlotCondition;
		pOutcomeMission->m_pPlotCondition = NULL;
	}

	if (!m_pUnitCondition)
	{
		m_pUnitCondition = pOutcomeMission->m_pUnitCondition;
		pOutcomeMission->m_pUnitCondition = NULL;
	}

	if (!m_iCost)
	{
		m_iCost = pOutcomeMission->m_iCost;
		pOutcomeMission->m_iCost = NULL;
	}
}

void CvOutcomeMission::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_eMission);
	CheckSum(iSum, m_bKill);
	CheckSum(iSum, m_ePayerType);
	m_PropertyCost.getCheckSum(iSum);
	m_OutcomeList.getCheckSum(iSum);
	if (m_iCost)
	{
		m_iCost->getCheckSum(iSum);
	}
}
