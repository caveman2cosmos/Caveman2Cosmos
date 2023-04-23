//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	BoolExpr.cpp
//
//  PURPOSE: Boolean Expressions for Civ4 classes
//
//------------------------------------------------------------------------------------------------

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvBonusInfo.h"
#include "CvBuildingInfo.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvBonusInfo.h"
#include "CvXMLLoadUtility.h"
#include "BoolExpr.h"
#include "IntExpr.h"
#include "CheckSum.h"
#include "CvImprovementInfo.h"
#include "FVariableSystem.h"
#include "wchar_utils.h"

static bool getBefore(BoolExprChange change)
{
	switch (change)
	{
	case BOOLEXPR_CHANGE_REMAINS_TRUE:
	case BOOLEXPR_CHANGE_BECOMES_FALSE:
		return true;

	default:
	case BOOLEXPR_CHANGE_REMAINS_FALSE:
	case BOOLEXPR_CHANGE_BECOMES_TRUE:
		return false;
	}
}

static bool getAfter(BoolExprChange change)
{
	switch (change)
	{
	case BOOLEXPR_CHANGE_REMAINS_TRUE:
	case BOOLEXPR_CHANGE_BECOMES_TRUE:
		return true;

	default:
	case BOOLEXPR_CHANGE_REMAINS_FALSE:
	case BOOLEXPR_CHANGE_BECOMES_FALSE:
		return false;
	}
}

static BoolExprChange getChange(bool before, bool after)
{
	if (before)
	{
		return after ? BOOLEXPR_CHANGE_REMAINS_TRUE : BOOLEXPR_CHANGE_BECOMES_FALSE;
	}
	else
	{
		return after ? BOOLEXPR_CHANGE_REMAINS_FALSE : BOOLEXPR_CHANGE_BECOMES_TRUE;
	}
}


BoolExpr::~BoolExpr()
{
}

const BoolExpr* BoolExpr::read(CvXMLLoadUtility *pXML)
{
	PROFILE_EXTRA_FUNC();
	// In general we assume no comments to simplify reading code

	//char szTag[1024];
	//if (!GETXML->GetLastLocatedNodeTagName(pXML->GetXML(), szTag))
	//{
	//	// No located node
	//	return NULL;
	//}

	//if (strcmp(szTag, "Not") == 0)
	//wchar_t szLog[1000];
	//swprintf(szLog, 1000, L"BoolExp '%s' ?= 'Not' is %i", pXML->GetXmlTagName(),
	//	     (int)equal(pXML->GetXmlTagName(), L"Not") );
	//OutputDebugStringW(szLog);

	if (equal(pXML->GetXmlTagName(), L"Not"))
	{
		// this is a Not node, read the first subexpression and generate a Not expression
		if (pXML->TryMoveToXmlFirstChild())
		{
			// there is a subexpression, so no simple constant
			const BoolExpr* pExpr = read(pXML);
			pXML->MoveToXmlParent();
			return new BoolExprNot(pExpr);
		}
		else
		{
			// constant, no need to generate a real Not
			BoolExprConstant* pExpr = new BoolExprConstant();
			pExpr->readConstant(pXML);
			return pExpr;
		}
	}

	//swprintf(szLog, 1000, L"BoolExp '%s' ?= 'And' is %i", pXML->GetXmlTagName(),
	//	     (int)equal(pXML->GetXmlTagName(), L"And") );
	//OutputDebugStringW(szLog);

	//if (strcmp(szTag, "And") == 0)
	if (equal(pXML->GetXmlTagName(), L"And"))
	{
		// this is an And node, check number of children
		if (pXML->GetXmlChildrenNumber() < 2)
		{
			// no real And node
			if (pXML->TryMoveToXmlFirstChild())
			{
				// there is a subexpression, so no simple constant
				const BoolExpr* pExpr = read(pXML);
				pXML->MoveToXmlParent();
				return pExpr;
			}
			else
			{
				// constant
				BoolExprConstant* pExpr = new BoolExprConstant();
				pExpr->readConstant(pXML);
				return pExpr;
			}
		}
		else
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				// read the first node
				const BoolExpr* pExpr = read(pXML);

				// read nodes until there are no more siblings
				while (pXML->TryMoveToXmlNextSibling())
				{
					pExpr = new BoolExprAnd(pExpr, read(pXML));
				}

				pXML->MoveToXmlParent();
				return pExpr;
			}
		}
	}

	if (equal(pXML->GetXmlTagName(), L"Or"))
	{
		// this is an Or node, check number of children
		if (pXML->GetXmlChildrenNumber() < 2)
		{
			// no real Or node
			if (pXML->TryMoveToXmlFirstChild())
			{
				// there is a subexpression, so no simple constant
				const BoolExpr* pExpr = read(pXML);
				pXML->MoveToXmlParent();
				return pExpr;
			}
			else
			{
				// constant
				BoolExprConstant* pExpr = new BoolExprConstant();
				pExpr->readConstant(pXML);
				return pExpr;
			}
		}
		else
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				// read the first node
				const BoolExpr* pExpr = read(pXML);

				// read nodes until there are no more siblings
				while (pXML->TryMoveToXmlNextSibling())
				{
					pExpr = new BoolExprOr(pExpr, read(pXML));
				}

				pXML->MoveToXmlParent();
				return pExpr;
			}
		}
	}

	if (equal(pXML->GetXmlTagName(), L"BEqual"))
	{
		// this is a Boolean Expression comparison node, check number of children
		if (pXML->GetXmlChildrenNumber() != 2)
		{
			// BEqual nodes must have two boolean expressions, make it a constant false node
			return new BoolExprConstant(false);
		}
		else
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				// read the first node
				const BoolExpr* pExpr = read(pXML);

				// read the second node
				if (pXML->TryMoveToXmlNextSibling())
				{
					pExpr = new BoolExprBEqual(pExpr, read(pXML));
				}

				pXML->MoveToXmlParent();
				return pExpr;
			}
		}
	}

	if (equal(pXML->GetXmlTagName(), L"Greater"))
	{
		// this is a comparison node, check number of children
		if (pXML->GetXmlChildrenNumber() != 2)
		{
			// Comparison nodes must have two boolean expressions, make it a constant false node
			return new BoolExprConstant(false);
		}
		else
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				// read the first node
				std::auto_ptr<const IntExpr> pExpr(IntExpr::read(pXML));
				const BoolExpr* pBExpr = NULL;

				// read the second node
				if (pXML->TryMoveToXmlNextSibling())
				{
					pBExpr = new BoolExprGreater(pExpr.release(), IntExpr::read(pXML));
				}

				pXML->MoveToXmlParent();
				return pBExpr;
			}
		}
	}

	if (equal(pXML->GetXmlTagName(), L"GreaterEqual"))
	{
		// this is a comparison node, check number of children
		if (pXML->GetXmlChildrenNumber() != 2)
		{
			// Comparison nodes must have two boolean expressions, make it a constant false node
			return new BoolExprConstant(false);
		}
		else
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				// read the first node
				std::auto_ptr<const IntExpr> pExpr(IntExpr::read(pXML));
				const BoolExpr* pBExpr = NULL;

				// read the second node
				if (pXML->TryMoveToXmlNextSibling())
				{
					pBExpr = new BoolExprGreaterEqual(pExpr.release(), IntExpr::read(pXML));
				}

				pXML->MoveToXmlParent();
				return pBExpr;
			}
		}
	}

	if (equal(pXML->GetXmlTagName(), L"Equal"))
	{
		// this is a comparison node, check number of children
		if (pXML->GetXmlChildrenNumber() != 2)
		{
			// Comparison nodes must have two boolean expressions, make it a constant false node
			return new BoolExprConstant(false);
		}
		else
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				// read the first node
				std::auto_ptr<const IntExpr> pExpr(IntExpr::read(pXML));
				const BoolExpr* pBExpr = NULL;

				// read the second node
				if (pXML->TryMoveToXmlNextSibling())
				{
					pBExpr = new BoolExprEqual(pExpr.release(), IntExpr::read(pXML));
				}

				pXML->MoveToXmlParent();
				return pBExpr;
			}
		}
	}

	if (equal(pXML->GetXmlTagName(), L"If"))
	{
		// this is an if/then/else node, check number of children
		if (pXML->GetXmlChildrenNumber() != 3)
		{
			// if/then/else nodes must have three boolean expressions, make it a constant false node
			return new BoolExprConstant(false);
		}
		else
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				// read the if node
				const BoolExpr* pIfExpr = read(pXML);
				const BoolExpr* pThenExpr = NULL;
				const BoolExpr* pElseExpr = NULL;

				// read the then node
				if (pXML->TryMoveToXmlNextSibling())
				{
					pThenExpr = read(pXML);
				}
				// read the else node
				if (pXML->TryMoveToXmlNextSibling())
				{
					pElseExpr = read(pXML);
				}

				pXML->MoveToXmlParent();
				return new BoolExprIf(pIfExpr, pThenExpr, pElseExpr);
			}
		}
	}

	// Check for the integrators
	if (equal(pXML->GetXmlTagName(), L"IntegrateOr"))
	{
		CvString szTextVal;
		pXML->GetChildXmlValByName(szTextVal, L"RelationType");
		const RelationTypes eRelation = (RelationTypes) pXML->GetInfoClass(szTextVal);
		int iData = -1;
		pXML->GetOptionalChildXmlValByName(&iData, L"iDistance", -1);
		pXML->GetChildXmlValByName(szTextVal, L"GameObjectType");
		const GameObjectTypes eType = (GameObjectTypes) pXML->GetInfoClass(szTextVal);

		// Find the expression and read it
		if (pXML->TryMoveToXmlFirstChild())
		{
			const BoolExpr* pExpr = NULL;
			do
			{
				//if (!GETXML->IsLastLocatedNodeCommentNode(pXML->GetXML()))
				//{
				//if (GETXML->GetLastLocatedNodeTagName(pXML->GetXML(), szInnerTag))
				//{
				if ( !(equal(pXML->GetXmlTagName(), L"RelationType")   ||
					   equal(pXML->GetXmlTagName(), L"iDistance")      ||
					   equal(pXML->GetXmlTagName(), L"GameObjectType")   ) )
				{
					pExpr = BoolExpr::read(pXML);
					break;
				}
				//}
				//}
			}
			while (pXML->TryMoveToXmlNextSibling());

			pXML->MoveToXmlParent();

			// If no subexpression is defined, use constant true (so it will be true if any related object is found)
			if (!pExpr)
				pExpr = new BoolExprConstant(true);

			return new BoolExprIntegrateOr(pExpr, eRelation, iData, eType);
		}
	}


	//swprintf(szLog, 1000, L"BoolExp '%s' ?= 'Has' is %i", pXML->GetXmlTagName(),
	//	     (int)equal(pXML->GetXmlTagName(), L"Has") );
	//OutputDebugStringW(szLog);

	// Check for the different literals
	if (equal(pXML->GetXmlTagName(), L"Has"))
	{
		// this is a Has GOM node
		BoolExprHas* pExpr = new BoolExprHas();
		pExpr->readContent(pXML);
		return pExpr;
	}

	if (equal(pXML->GetXmlTagName(), L"Is"))
	{
		// this is an Is node, querying a tag of the game object
		CvString szTextVal;
		pXML->GetXmlVal(szTextVal);
		return new BoolExprIs((TagTypes)pXML->GetInfoClass(szTextVal));
	}

	if (equal(pXML->GetXmlTagName(), L"Constant"))
	{
		// this is an Constant node
		BoolExprConstant* pExpr = new BoolExprConstant();
		pExpr->readConstant(pXML);
		return pExpr;
	}

	// Check if there is a subexpression and use that
	if (pXML->TryMoveToXmlFirstChild())
	{
		// there is a subexpression, so no simple constant
		const BoolExpr* pExpr = read(pXML);
		pXML->MoveToXmlParent();
		return pExpr;
	}

	else
	{
		// constant
		BoolExprConstant* pExpr = new BoolExprConstant();
		pExpr->readConstant(pXML);
		return pExpr;
	}
}

bool BoolExprConstant::evaluate(const CvGameObject* pObject) const
{
	return m_bValue;
}

BoolExprChange BoolExprConstant::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	return m_bValue ? BOOLEXPR_CHANGE_REMAINS_TRUE : BOOLEXPR_CHANGE_REMAINS_FALSE;
}

bool BoolExprConstant::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	return false;
}

void BoolExprConstant::readConstant(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

	pXML->GetXmlVal(szTextVal);
	if (!GC.getDefinesVarSystem()->GetValue(szTextVal, m_bValue))
	{
		pXML->GetXmlVal(&m_bValue);
	}

}

void BoolExprConstant::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(m_bValue ? gDLL->getText("TXT_KEY_EXPR_TRUE") : gDLL->getText("TXT_KEY_EXPR_FALSE"));
}

int BoolExprConstant::getBindingStrength() const
{
	return 100;
}

void BoolExprConstant::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bValue);
}



BoolExprHas::~BoolExprHas()
{
	GC.removeDelayedResolution(&m_iID);
}

bool BoolExprHas::evaluate(const CvGameObject* pObject) const
{
	return pObject->hasGOM(m_eGOM, m_iID);
}

BoolExprChange BoolExprHas::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	PROFILE_EXTRA_FUNC();
	const bool result = pObject->hasGOM(m_eGOM, m_iID);
	foreach_(const GOMOverride& it, overrides)
	{
		if (it.pObject == pObject && it.GOM == m_eGOM && it.id == m_iID)
		{
			if (it.bHas == result)
			{
				break; // does not change expression result
			}
			return it.bHas ? BOOLEXPR_CHANGE_BECOMES_TRUE : BOOLEXPR_CHANGE_BECOMES_FALSE;
		}
	}
	return result ? BOOLEXPR_CHANGE_REMAINS_TRUE : BOOLEXPR_CHANGE_REMAINS_FALSE;
}

bool BoolExprHas::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const GOMQuery& it, queries)
	{
		if (it.GOM == m_eGOM && it.id == m_iID)
		{
			return true;
		}
	}
	return false;
}

void BoolExprHas::readContent(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, L"GOMType");
	m_eGOM = (GOMTypes)pXML->GetInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, L"ID");
	GC.addDelayedResolution(&m_iID, szTextVal);
}

void BoolExprHas::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	switch (m_eGOM)
	{
		case GOM_BUILDING:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getBuildingInfo((BuildingTypes)m_iID).getType()).GetCString(), GC.getBuildingInfo((BuildingTypes)m_iID).getDescription()));
			break;
		case GOM_PROMOTION:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getPromotionInfo((PromotionTypes)m_iID).getType()).GetCString(), GC.getPromotionInfo((PromotionTypes)m_iID).getDescription()));
			break;
		case GOM_TRAIT:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getTraitInfo((TraitTypes)m_iID).getType()).GetCString(), GC.getTraitInfo((TraitTypes)m_iID).getDescription()));
			break;
		case GOM_FEATURE:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getFeatureInfo((FeatureTypes)m_iID).getType()).GetCString(), GC.getFeatureInfo((FeatureTypes)m_iID).getDescription()));
			break;
		case GOM_OPTION:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getGameOptionInfo((GameOptionTypes)m_iID).getType()).GetCString(), GC.getGameOptionInfo((GameOptionTypes)m_iID).getDescription()));
			break;
		case GOM_TERRAIN:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getTerrainInfo((TerrainTypes)m_iID).getType()).GetCString(), GC.getTerrainInfo((TerrainTypes)m_iID).getDescription()));
			break;
		case GOM_GAMESPEED:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getGameSpeedInfo((GameSpeedTypes)m_iID).getType()).GetCString(), GC.getGameSpeedInfo((GameSpeedTypes)m_iID).getDescription()));
			break;
		case GOM_ROUTE:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getRouteInfo((RouteTypes)m_iID).getType()).GetCString(), GC.getRouteInfo((RouteTypes)m_iID).getDescription()));
			break;
		case GOM_BONUS:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getBonusInfo((BonusTypes)m_iID).getType()).GetCString(), GC.getBonusInfo((BonusTypes)m_iID).getDescription()));
			break;
		case GOM_UNITTYPE:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getUnitInfo((UnitTypes)m_iID).getType()).GetCString(), GC.getUnitInfo((UnitTypes)m_iID).getDescription()));
			break;
		case GOM_TECH:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getTechInfo((TechTypes)m_iID).getType()).GetCString(), GC.getTechInfo((TechTypes)m_iID).getDescription()));
			break;
		case GOM_CIVIC:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getCivicInfo((CivicTypes)m_iID).getType()).GetCString(), GC.getCivicInfo((CivicTypes)m_iID).getDescription()));
			break;
		case GOM_RELIGION:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getReligionInfo((ReligionTypes)m_iID).getType()).GetCString(), GC.getReligionInfo((ReligionTypes)m_iID).getDescription()));
			break;
		case GOM_CORPORATION:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getCorporationInfo((CorporationTypes)m_iID).getType()).GetCString(), GC.getCorporationInfo((CorporationTypes)m_iID).getDescription()));
			break;
		case GOM_IMPROVEMENT:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getImprovementInfo((ImprovementTypes)m_iID).getType()).GetCString(), GC.getImprovementInfo((ImprovementTypes)m_iID).getDescription()));
			break;
		case GOM_UNITCOMBAT:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getUnitCombatInfo((UnitCombatTypes)m_iID).getType()).GetCString(), GC.getUnitCombatInfo((UnitCombatTypes)m_iID).getDescription()));
			break;
		case GOM_HANDICAP:
			szBuffer.append(CvWString::format(L"<link=%s>%s</link>", CvWString(GC.getHandicapInfo((HandicapTypes)m_iID).getType()).GetCString(), GC.getHandicapInfo((HandicapTypes)m_iID).getDescription()));
			break;
	}
}

int BoolExprHas::getBindingStrength() const
{
	return 100;
}

void BoolExprHas::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, (int)m_eGOM);
	CheckSum(iSum, m_iID);
}


bool BoolExprIs::evaluate(const CvGameObject* pObject) const
{
	return pObject->isTag(m_eTag);
}

BoolExprChange BoolExprIs::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	return pObject->isTag(m_eTag) ? BOOLEXPR_CHANGE_REMAINS_TRUE : BOOLEXPR_CHANGE_REMAINS_FALSE;
}

bool BoolExprIs::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	return false;
}

void BoolExprIs::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	// This is only the quick and dirty variant. Needs some proper text usage still.
	switch (m_eTag)
	{
		case TAG_WATER:
			szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IS_WATER"));
			break;
		case TAG_FRESH_WATER:
			szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IS_FRESH_WATER"));
			break;
		case TAG_PEAK:
			szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IS_PEAK"));
			break;
		case TAG_HILL:
			szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IS_HILL"));
			break;
		case TAG_FLATLAND:
			szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IS_FLAT_LAND"));
			break;
		case TAG_OWNED:
			szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IS_OWNED"));
			break;
		case TAG_CITY:
			szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IS_CITY"));
			break;
		case TAG_ANARCHY:
			szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IS_IN_ANARCHY"));
			break;
		case TAG_COASTAL:
			szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IS_COASTAL"));
			break;
	}
}

int BoolExprIs::getBindingStrength() const
{
	return 100;
}

void BoolExprIs::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, (int)m_eTag);
}



BoolExprNot::~BoolExprNot()
{
	SAFE_DELETE(m_pExpr);
}

bool BoolExprNot::evaluate(const CvGameObject* pObject) const
{
	return !m_pExpr->evaluate(pObject);
}

BoolExprChange BoolExprNot::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	const BoolExprChange result = m_pExpr->evaluateChange(pObject, overrides);
	switch (result)
	{
	default:
	case BOOLEXPR_CHANGE_REMAINS_TRUE:
		return BOOLEXPR_CHANGE_REMAINS_FALSE;

	case BOOLEXPR_CHANGE_REMAINS_FALSE:
		return BOOLEXPR_CHANGE_REMAINS_TRUE;

	case BOOLEXPR_CHANGE_BECOMES_TRUE:
		return BOOLEXPR_CHANGE_BECOMES_FALSE;

	case BOOLEXPR_CHANGE_BECOMES_FALSE:
		return BOOLEXPR_CHANGE_BECOMES_TRUE;
	}
}

bool BoolExprNot::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	return m_pExpr->getInvolvesGOM(queries);
}

void BoolExprNot::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(gDLL->getText("TXT_KEY_EXPR_NOT_"));
	m_pExpr->buildDisplayString(szBuffer);
	szBuffer.append(")");
}

int BoolExprNot::getBindingStrength() const
{
	return 40;
}

void BoolExprNot::getCheckSum(uint32_t& iSum) const
{
	m_pExpr->getCheckSum(iSum);
}


BoolExprAnd::~BoolExprAnd()
{
	SAFE_DELETE(m_pExpr1);
	SAFE_DELETE(m_pExpr2);
}

bool BoolExprAnd::evaluate(const CvGameObject* pObject) const
{
	return m_pExpr1->evaluate(pObject) && m_pExpr2->evaluate(pObject);
}

BoolExprChange BoolExprAnd::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	const BoolExprChange result1 = m_pExpr1->evaluateChange(pObject, overrides);
	if (result1 == BOOLEXPR_CHANGE_REMAINS_FALSE)
	{
		return BOOLEXPR_CHANGE_REMAINS_FALSE;
	}
	const BoolExprChange result2 = m_pExpr2->evaluateChange(pObject, overrides);
	if (result2 == BOOLEXPR_CHANGE_REMAINS_FALSE)
	{
		return BOOLEXPR_CHANGE_REMAINS_FALSE;
	}
	if (result1 == BOOLEXPR_CHANGE_REMAINS_TRUE)
	{
		return result2;
	}
	if (result2 == BOOLEXPR_CHANGE_REMAINS_TRUE)
	{
		return result1;
	}
	if (result1 != result2)
	{
		return BOOLEXPR_CHANGE_REMAINS_FALSE;
	}
	return result1;
}

bool BoolExprAnd::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	return m_pExpr1->getInvolvesGOM(queries) || m_pExpr2->getInvolvesGOM(queries);
}

void BoolExprAnd::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	const bool bBrackets1 = getBindingStrength() > m_pExpr1->getBindingStrength();
	const bool bBrackets2 = getBindingStrength() > m_pExpr2->getBindingStrength();

	if (bBrackets1) szBuffer.append("(");
	m_pExpr1->buildDisplayString(szBuffer);
	if (bBrackets1) szBuffer.append(")");

	szBuffer.append(gDLL->getText("TXT_KEY_EXPR_AND"));

	if (bBrackets2) szBuffer.append("(");
	m_pExpr2->buildDisplayString(szBuffer);
	if (bBrackets2) szBuffer.append(")");
}

int BoolExprAnd::getBindingStrength() const
{
	return 20;
}

void BoolExprAnd::getCheckSum(uint32_t& iSum) const
{
	m_pExpr1->getCheckSum(iSum);
	m_pExpr2->getCheckSum(iSum);
}



BoolExprOr::~BoolExprOr()
{
	SAFE_DELETE(m_pExpr1);
	SAFE_DELETE(m_pExpr2);
}

bool BoolExprOr::evaluate(const CvGameObject* pObject) const
{
	return m_pExpr1->evaluate(pObject) || m_pExpr2->evaluate(pObject);
}

BoolExprChange BoolExprOr::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	const BoolExprChange result1 = m_pExpr1->evaluateChange(pObject, overrides);
	if (result1 == BOOLEXPR_CHANGE_REMAINS_TRUE)
	{
		return BOOLEXPR_CHANGE_REMAINS_TRUE;
	}
	const BoolExprChange result2 = m_pExpr2->evaluateChange(pObject, overrides);
	if (result2 == BOOLEXPR_CHANGE_REMAINS_TRUE)
	{
		return BOOLEXPR_CHANGE_REMAINS_TRUE;
	}
	if (result1 == BOOLEXPR_CHANGE_REMAINS_FALSE)
	{
		return result2;
	}
	if (result2 == BOOLEXPR_CHANGE_REMAINS_FALSE)
	{
		return result1;
	}
	if (result1 != result2)
	{
		return BOOLEXPR_CHANGE_REMAINS_TRUE;
	}
	return result1;
}

bool BoolExprOr::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	return m_pExpr1->getInvolvesGOM(queries) || m_pExpr2->getInvolvesGOM(queries);
}

void BoolExprOr::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	const bool bBrackets1 = getBindingStrength() > m_pExpr1->getBindingStrength();
	const bool bBrackets2 = getBindingStrength() > m_pExpr2->getBindingStrength();

	if (bBrackets1) szBuffer.append("(");
	m_pExpr1->buildDisplayString(szBuffer);
	if (bBrackets1) szBuffer.append(")");

	szBuffer.append(gDLL->getText("TXT_KEY_EXPR_OR"));

	if (bBrackets2) szBuffer.append("(");
	m_pExpr2->buildDisplayString(szBuffer);
	if (bBrackets2) szBuffer.append(")");
}

int BoolExprOr::getBindingStrength() const
{
	return 10;
}

void BoolExprOr::getCheckSum(uint32_t& iSum) const
{
	m_pExpr1->getCheckSum(iSum);
	m_pExpr2->getCheckSum(iSum);
}


BoolExprBEqual::~BoolExprBEqual()
{
	SAFE_DELETE(m_pExpr1);
	SAFE_DELETE(m_pExpr2);
}

bool BoolExprBEqual::evaluate(const CvGameObject* pObject) const
{
	return m_pExpr1->evaluate(pObject) == m_pExpr2->evaluate(pObject);
}

BoolExprChange BoolExprBEqual::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	const BoolExprChange result1 = m_pExpr1->evaluateChange(pObject, overrides);
	const BoolExprChange result2 = m_pExpr2->evaluateChange(pObject, overrides);
	if (result1 == result2)
	{
		return BOOLEXPR_CHANGE_REMAINS_TRUE;
	}
	if (result1 == BOOLEXPR_CHANGE_REMAINS_TRUE)
	{
		return result2;
	}
	if (result2 == BOOLEXPR_CHANGE_REMAINS_TRUE)
	{
		return result1;
	}
	if (result1 == BOOLEXPR_CHANGE_REMAINS_FALSE)
	{
		return (result2 == BOOLEXPR_CHANGE_BECOMES_FALSE) ? BOOLEXPR_CHANGE_BECOMES_TRUE : BOOLEXPR_CHANGE_BECOMES_FALSE;
	}
	if (result2 == BOOLEXPR_CHANGE_REMAINS_FALSE)
	{
		return (result1 == BOOLEXPR_CHANGE_BECOMES_FALSE) ? BOOLEXPR_CHANGE_BECOMES_TRUE : BOOLEXPR_CHANGE_BECOMES_FALSE;
	}
	return BOOLEXPR_CHANGE_REMAINS_FALSE;
}

bool BoolExprBEqual::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	return m_pExpr1->getInvolvesGOM(queries) || m_pExpr2->getInvolvesGOM(queries);
}

void BoolExprBEqual::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	const bool bBrackets1 = getBindingStrength() > m_pExpr1->getBindingStrength();
	const bool bBrackets2 = getBindingStrength() > m_pExpr2->getBindingStrength();

	if (bBrackets1) szBuffer.append("(");
	m_pExpr1->buildDisplayString(szBuffer);
	if (bBrackets1) szBuffer.append(")");

	szBuffer.append(gDLL->getText("TXT_KEY_EXPR_EQUALS"));

	if (bBrackets2) szBuffer.append("(");
	m_pExpr2->buildDisplayString(szBuffer);
	if (bBrackets2) szBuffer.append(")");
}

int BoolExprBEqual::getBindingStrength() const
{
	return 30;
}

void BoolExprBEqual::getCheckSum(uint32_t& iSum) const
{
	m_pExpr1->getCheckSum(iSum);
	m_pExpr2->getCheckSum(iSum);
}


BoolExprIf::~BoolExprIf()
{
	SAFE_DELETE(m_pExprIf);
	SAFE_DELETE(m_pExprThen);
	SAFE_DELETE(m_pExprElse);
}

bool BoolExprIf::evaluate(const CvGameObject* pObject) const
{
	return m_pExprIf->evaluate(pObject) ? m_pExprThen->evaluate(pObject) : m_pExprElse->evaluate(pObject);
}

BoolExprChange BoolExprIf::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	BoolExprChange resultif = m_pExprIf->evaluateChange(pObject, overrides);
	BoolExprChange resultthen = m_pExprThen->evaluateChange(pObject, overrides);
	BoolExprChange resultelse = m_pExprElse->evaluateChange(pObject, overrides);

	bool before = getBefore(resultif) ? getBefore(resultthen) : getBefore(resultelse);
	bool after = getAfter(resultif) ? getAfter(resultthen) : getAfter(resultelse);
	return getChange(before, after);
}

bool BoolExprIf::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	return m_pExprIf->getInvolvesGOM(queries) || m_pExprThen->getInvolvesGOM(queries) || m_pExprElse->getInvolvesGOM(queries);
}

void BoolExprIf::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	const bool bBracketsIf = getBindingStrength() > m_pExprIf->getBindingStrength();
	const bool bBracketsThen = getBindingStrength() > m_pExprThen->getBindingStrength();
	const bool bBracketsElse = getBindingStrength() > m_pExprElse->getBindingStrength();

	szBuffer.append(gDLL->getText("TXT_KEY_EXPR_IF"));
	if (bBracketsIf) szBuffer.append("(");
	m_pExprIf->buildDisplayString(szBuffer);
	if (bBracketsIf) szBuffer.append(")");

	szBuffer.append(gDLL->getText("TXT_KEY_EXPR_THEN"));
	if (bBracketsThen) szBuffer.append("(");
	m_pExprThen->buildDisplayString(szBuffer);
	if (bBracketsThen) szBuffer.append(")");

	szBuffer.append(gDLL->getText("TXT_KEY_EXPR_ELSE"));
	if (bBracketsElse) szBuffer.append("(");
	m_pExprElse->buildDisplayString(szBuffer);
	if (bBracketsElse) szBuffer.append(")");
}

int BoolExprIf::getBindingStrength() const
{
	return 25;
}

void BoolExprIf::getCheckSum(uint32_t& iSum) const
{
	m_pExprIf->getCheckSum(iSum);
	m_pExprThen->getCheckSum(iSum);
	m_pExprElse->getCheckSum(iSum);
}


void evalExprIntegrateOr(const CvGameObject* pObject, const BoolExpr* pExpr, bool* bAcc)
{
	*bAcc = *bAcc || pExpr->evaluate(pObject);
}

BoolExprIntegrateOr::~BoolExprIntegrateOr()
{
	SAFE_DELETE(m_pExpr);
}

bool BoolExprIntegrateOr::evaluate(const CvGameObject* pObject) const
{
	PROFILE_EXTRA_FUNC();
	bool bAcc = false;
	pObject->foreachRelated(m_eType, m_eRelation, bind(evalExprIntegrateOr, _1, m_pExpr, &bAcc));
	return bAcc;
}

void evalExprChangeIntegrateOr(const CvGameObject* pObject, const BoolExpr* pExpr, BoolExprChange* bAcc, const std::vector<GOMOverride>& overrides)
{
	if (*bAcc == BOOLEXPR_CHANGE_REMAINS_TRUE)
	{
		return;
	}
	const BoolExprChange change = pExpr->evaluateChange(pObject, overrides);
	if (change == BOOLEXPR_CHANGE_REMAINS_TRUE)
	{
		*bAcc = BOOLEXPR_CHANGE_REMAINS_TRUE;
		return;
	}
	switch (*bAcc)
	{
	case BOOLEXPR_CHANGE_REMAINS_FALSE:
		*bAcc = change;
		return;

	case BOOLEXPR_CHANGE_BECOMES_FALSE:
		if (change == BOOLEXPR_CHANGE_BECOMES_TRUE)
		{
			*bAcc = BOOLEXPR_CHANGE_BECOMES_TRUE;
		}
	}
}

BoolExprChange BoolExprIntegrateOr::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	PROFILE_EXTRA_FUNC();
	BoolExprChange bAcc = BOOLEXPR_CHANGE_REMAINS_FALSE;
	pObject->foreachRelated(m_eType, m_eRelation, bind(evalExprChangeIntegrateOr, _1, m_pExpr, &bAcc, overrides));
	return bAcc;
}

bool BoolExprIntegrateOr::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	return m_pExpr->getInvolvesGOM(queries);
}

void BoolExprIntegrateOr::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	// TODO: Proper rendering of relations
	m_pExpr->buildDisplayString(szBuffer);
}

int BoolExprIntegrateOr::getBindingStrength() const
{
	// TODO: Update when proper buildDisplayString is implemented
	return m_pExpr->getBindingStrength();
}

void BoolExprIntegrateOr::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, (int)m_eRelation);
	CheckSum(iSum, m_iData);
	CheckSum(iSum, (int)m_eType);
	m_pExpr->getCheckSum(iSum);
}


BoolExprComp::~BoolExprComp()
{
	SAFE_DELETE(m_pExpr1);
	SAFE_DELETE(m_pExpr2);
}

BoolExprChange BoolExprComp::evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const
{
	// we do not currently trace changes indirectly over int expressions
	// so just use the normal evaluation and assume remains
	return evaluate(pObject) ? BOOLEXPR_CHANGE_REMAINS_TRUE : BOOLEXPR_CHANGE_REMAINS_FALSE;
}

bool BoolExprComp::getInvolvesGOM(const std::vector<GOMQuery>& queries) const
{
	return false;
}

void BoolExprComp::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	const bool bBrackets1 = getBindingStrength() > m_pExpr1->getBindingStrength();
	const bool bBrackets2 = getBindingStrength() > m_pExpr2->getBindingStrength();

	if (bBrackets1) szBuffer.append("(");
	m_pExpr1->buildDisplayString(szBuffer);
	if (bBrackets1) szBuffer.append(")");

	buildOpNameString(szBuffer);

	if (bBrackets2) szBuffer.append("(");
	m_pExpr2->buildDisplayString(szBuffer);
	if (bBrackets2) szBuffer.append(")");
}

int BoolExprComp::getBindingStrength() const
{
	return 5;
}

void BoolExprComp::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, (int)getType());
	m_pExpr1->getCheckSum(iSum);
	m_pExpr2->getCheckSum(iSum);
}


bool BoolExprGreater::evaluate(const CvGameObject* pObject) const
{
	return m_pExpr1->evaluate(pObject) > m_pExpr2->evaluate(pObject);
}

BoolExprTypes BoolExprGreater::getType() const
{
	return BOOLEXPR_GREATER;
}

void BoolExprGreater::buildOpNameString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(" > ");
}


bool BoolExprGreaterEqual::evaluate(const CvGameObject* pObject) const
{
	return m_pExpr1->evaluate(pObject) >= m_pExpr2->evaluate(pObject);
}

BoolExprTypes BoolExprGreaterEqual::getType() const
{
	return BOOLEXPR_GREATER_EQUAL;
}

void BoolExprGreaterEqual::buildOpNameString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(" >= ");
}


bool BoolExprEqual::evaluate(const CvGameObject* pObject) const
{
	return m_pExpr1->evaluate(pObject) == m_pExpr2->evaluate(pObject);
}

BoolExprTypes BoolExprEqual::getType() const
{
	return BOOLEXPR_EQUAL;
}

void BoolExprEqual::buildOpNameString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(" = ");
}
