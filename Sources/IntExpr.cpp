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
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLPythonIFaceBase.h"
#include "CheckSum.h"
#include "IntExpr.h"

IntExpr::~IntExpr()
{
}

const IntExpr* IntExpr::read(CvXMLLoadUtility *pXML)
{
	PROFILE_EXTRA_FUNC();
	// In general we assume no comments to simplify reading code

	//TCHAR szTag[1024];
	const wchar_t* szTag = pXML->GetXmlTagName();

	//if (!GETXML->GetLastLocatedNodeTagName(pXML->GetXML(), szTag))
	//{
	//	// No located node
	//	return NULL;
	//}

	if (wcscmp(szTag, L"Random") == 0)
	{
		// this is a Random node, check number of children
		if (pXML->GetXmlChildrenNumber() < 1)
		{
			// Random node with constant
			int iConstant = 0;
			pXML->GetXmlVal(&iConstant);
			return new IntExprRandom(new IntExprConstant(iConstant));
		}
		if (pXML->TryMoveToXmlFirstChild())
		{
			// read the subnode
			const IntExpr* pExpr = read(pXML);

			pXML->MoveToXmlParent();
			return new IntExprRandom(pExpr);
		}
	}

	if (wcscmp(szTag, L"Adapt") == 0)
	{
		// this is an Adapt node, check number of children
		int iNumChildren = pXML->GetXmlChildrenNumber();
		if (iNumChildren < 1)
		{
			// Adapt node with constant and default adaption
			int iConstant = 0;
			pXML->GetXmlVal(&iConstant);
			return new IntExprAdapt(new IntExprConstant(iConstant));
		}
		if (iNumChildren < 2)
		{
			// one child, so adapt node with subexpression
			if (pXML->TryMoveToXmlFirstChild())
			{
				// read the subnode
				const IntExpr* pExpr = read(pXML);

				pXML->MoveToXmlParent();
				return new IntExprAdapt(pExpr);
			}
		}
		// first child is ID, second is subexpression
		else if (pXML->TryMoveToXmlFirstChild())
		{
			CvString szTextVal;
			pXML->GetXmlVal(szTextVal);

			if (!pXML->TryMoveToXmlNextSibling())
			{
				FErrorMsg("Adapt usb expression is not correctly constructed");
			}
			// read the subnode
			const IntExpr* pExpr = read(pXML);

			pXML->MoveToXmlParent();
			return new IntExprAdapt(pExpr, GC.getOrCreateInfoTypeForString(szTextVal));
		}
	}

	if (wcscmp(szTag, L"Plus") == 0)
	{
		// this is a Plus node, check number of children
		if (pXML->GetXmlChildrenNumber() < 2)
		{
			// no real Plus node
			if (pXML->TryMoveToXmlFirstChild())
			{
				// there is a subexpression, so no simple constant
				const IntExpr* pExpr = read(pXML);
				pXML->MoveToXmlParent();
				return pExpr;
			}
			// constant
			int iConstant = 0;
			pXML->GetXmlVal(&iConstant);
			return new IntExprConstant(iConstant);
		}
		if (pXML->TryMoveToXmlFirstChild())
		{
			// read the first node
			const IntExpr* pExpr = read(pXML);

			// read nodes until there are no more siblings
			while (pXML->TryMoveToXmlNextSibling())
			{
				pExpr = new IntExprPlus(pExpr, read(pXML));
			}
			pXML->MoveToXmlParent();
			return pExpr;
		}
	}

	if (wcscmp(szTag, L"Mult") == 0)
	{
		// this is a Mult node, check number of children
		if (pXML->GetXmlChildrenNumber() < 2)
		{
			// no real Mult node
			if (pXML->TryMoveToXmlFirstChild())
			{
				// there is a subexpression, so no simple constant
				const IntExpr* pExpr = read(pXML);
				pXML->MoveToXmlParent();
				return pExpr;
			}
			// constant
			int iConstant = 0;
			pXML->GetXmlVal(&iConstant);
			return new IntExprConstant(iConstant);
		}
		if (pXML->TryMoveToXmlFirstChild())
		{
			// read the first node
			const IntExpr* pExpr = read(pXML);

			// read nodes until there are no more siblings
			while (pXML->TryMoveToXmlNextSibling())
			{
				pExpr = new IntExprMult(pExpr, read(pXML));
			}
			pXML->MoveToXmlParent();
			return pExpr;
		}
	}

	if (wcscmp(szTag, L"Minus") == 0)
	{
		// this is a Minus node, check number of children
		if (pXML->GetXmlChildrenNumber() != 2)
		{
			// Minus nodes must have two subexpressions, make it a constant 0 node
			return new IntExprConstant(0);
		}
		if (pXML->TryMoveToXmlFirstChild())
		{
			// read the first node
			const IntExpr* pExpr = read(pXML);

			// read the second node
			if (pXML->TryMoveToXmlNextSibling())
			{
				pExpr = new IntExprMinus(pExpr, read(pXML));
			}
			pXML->MoveToXmlParent();
			return pExpr;
		}
	}

	if (wcscmp(szTag, L"Div") == 0)
	{
		// this is a Div node, check number of children
		if (pXML->GetXmlChildrenNumber() != 2)
		{
			// Div nodes must have two subexpressions, make it a constant 0 node
			return new IntExprConstant(0);
		}
		if (pXML->TryMoveToXmlFirstChild())
		{
			// read the first node
			const IntExpr* pExpr = read(pXML);

			// read the second node
			if (pXML->TryMoveToXmlNextSibling())
			{
				pExpr = new IntExprDiv(pExpr, read(pXML));
			}
			pXML->MoveToXmlParent();
			return pExpr;
		}
	}

	if (wcscmp(szTag, L"If") == 0)
	{
		// this is an if/then/else node, check number of children
		if (pXML->GetXmlChildrenNumber() != 3)
		{
			// if/then/else nodes must have three subexpressions, make it a constant 0 node
			return new IntExprConstant(0);
		}
		if (pXML->TryMoveToXmlFirstChild())
		{
			// read the if node
			const BoolExpr* pIfExpr = BoolExpr::read(pXML);
			const IntExpr* pThenExpr = NULL;
			const IntExpr* pElseExpr = NULL;

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
			return new IntExprIf(pIfExpr, pThenExpr, pElseExpr);
		}
	}

	// Check for the integrators
	if (wcscmp(szTag, L"IntegrateSum") == 0
	||  wcscmp(szTag, L"IntegrateAvg") == 0
	||  wcscmp(szTag, L"IntegrateCount") == 0)
	{
		CvString szTextVal;
		pXML->GetChildXmlValByName(szTextVal, L"RelationType");
		const RelationTypes eRelation = (RelationTypes) pXML->GetInfoClass(szTextVal);
		int iData = -1;
		pXML->GetOptionalChildXmlValByName(&iData, L"iDistance");
		pXML->GetChildXmlValByName(szTextVal, L"GameObjectType");
		const GameObjectTypes eType = (GameObjectTypes) pXML->GetInfoClass(szTextVal);

		const IntExpr* pExpr = NULL;
		const BoolExpr* pBExpr = NULL;
		// Find the expression and read it
		if (pXML->TryMoveToXmlFirstChild())
		{
			while (pXML->TryMoveToXmlNextSibling())
			{
				const wchar_t* szInnerTag = pXML->GetXmlTagName();

				if (wcscmp(szInnerTag, L"RelationType") != 0
				&&  wcscmp(szInnerTag, L"iDistance") != 0
				&&  wcscmp(szInnerTag, L"GameObjectType") != 0)
				{
					if (wcscmp(szTag, L"IntegrateCount") == 0)
					{
						pBExpr = BoolExpr::read(pXML);
					}
					else pExpr = IntExpr::read(pXML);

					break;
				}
			}
			pXML->MoveToXmlParent();
			if (wcscmp(szTag, L"IntegrateSum") == 0)
			{
				return new IntExprIntegrateSum(pExpr, eRelation, iData, eType);
			}
			if (wcscmp(szTag, L"IntegrateAvg") == 0)
			{
				return new IntExprIntegrateAvg(pExpr, eRelation, iData, eType);
			}
			return new IntExprIntegrateCount(pBExpr, eRelation, iData, eType);
		}
	}

	// Check for the different literals
	if (wcscmp(szTag, L"PropertyType") == 0)
	{
		// this is a Property node, querying a property of the game object
		CvString szTextVal;
		pXML->GetXmlVal(szTextVal);
		return new IntExprProperty((PropertyTypes)pXML->GetInfoClass(szTextVal));
	}

	if (wcscmp(szTag, L"AttributeType") == 0)
	{
		// this is an Attribute node, querying an attribute of the game object
		CvString szTextVal;
		pXML->GetXmlVal(szTextVal);
		return new IntExprAttribute((AttributeTypes)pXML->GetInfoClass(szTextVal));
	}

	if (wcscmp(szTag, L"Python") == 0)
	{
		// this is a Python node
		CvString szTextVal;
		pXML->GetXmlVal(szTextVal);
		return new IntExprPython(szTextVal);
	}

	// Check if there is a subexpression and use that
	if (pXML->TryMoveToXmlFirstChild())
	{
		// there is a subexpression, so no simple constant
		const IntExpr* pExpr = read(pXML);
		pXML->MoveToXmlParent();
		return pExpr;
	}
	// constant
	int iConstant = 0;
	pXML->GetXmlVal(&iConstant);
	return new IntExprConstant(iConstant);
}

bool IntExpr::isConstantZero() const
{
	return false;
}


int IntExprConstant::evaluate(const CvGameObject* pObject) const
{
	return m_iValue;
}

void IntExprConstant::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString s;
	s.Format(L"%+d", m_iValue);
	szBuffer.append(s);
}

int IntExprConstant::getBindingStrength() const
{
	return 100;
}

bool IntExprConstant::isConstantZero() const
{
	return m_iValue == 0;
}

void IntExprConstant::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iValue);
}


int IntExprAttribute::evaluate(const CvGameObject* pObject) const
{
	return pObject->getAttribute(m_eAttribute);
}

void IntExprAttribute::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString s;
	switch (m_eAttribute)
	{
		case ATTRIBUTE_POPULATION:
			s.Format(L"%c", gDLL->getSymbolID(CITIZEN_CHAR));
			szBuffer.append(s);
			break;
		case ATTRIBUTE_HEALTH:
			s.Format(L"%c", gDLL->getSymbolID(HEALTHY_CHAR));
			szBuffer.append(s);
			break;
		case ATTRIBUTE_HAPPINESS:
			s.Format(L"%c", gDLL->getSymbolID(HAPPY_CHAR));
			szBuffer.append(s);
			break;
		case ATTRIBUTE_PLAYERS:
			szBuffer.append("Players");
			break;
		case ATTRIBUTE_TEAMS:
			szBuffer.append("Teams");
			break;
	}
}

int IntExprAttribute::getBindingStrength() const
{
	return 100;
}

void IntExprAttribute::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, (int)m_eAttribute);
}


int IntExprProperty::evaluate(const CvGameObject* pObject) const
{
	return pObject->getProperties()->getValueByProperty(m_eProperty);
}

void IntExprProperty::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString s;
	s.Format(L"%c", GC.getPropertyInfo(m_eProperty).getChar());
	szBuffer.append(s);
}

int IntExprProperty::getBindingStrength() const
{
	return 100;
}

void IntExprProperty::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, (int)m_eProperty);
}


IntExprOp::~IntExprOp()
{
	SAFE_DELETE(m_pExpr1);
	SAFE_DELETE(m_pExpr2);
}

void IntExprOp::buildDisplayString(CvWStringBuffer &szBuffer) const
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

void IntExprOp::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, (int)getType());
	m_pExpr1->getCheckSum(iSum);
	m_pExpr2->getCheckSum(iSum);
}


int IntExprPlus::evaluate(const CvGameObject* pObject) const
{
	return m_pExpr1->evaluate(pObject) + m_pExpr2->evaluate(pObject);
}

IntExprTypes IntExprPlus::getType() const
{
	return INTEXPR_PLUS;
}

void IntExprPlus::buildOpNameString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(" + ");
}

int IntExprPlus::getBindingStrength() const
{
	return 11;
}


int IntExprMinus::evaluate(const CvGameObject* pObject) const
{
	return m_pExpr1->evaluate(pObject) - m_pExpr2->evaluate(pObject);
}

IntExprTypes IntExprMinus::getType() const
{
	return INTEXPR_MINUS;
}

void IntExprMinus::buildOpNameString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(" - ");
}

int IntExprMinus::getBindingStrength() const
{
	return 11;
}


int IntExprMult::evaluate(const CvGameObject* pObject) const
{
	return m_pExpr1->evaluate(pObject) * m_pExpr2->evaluate(pObject);
}

IntExprTypes IntExprMult::getType() const
{
	return INTEXPR_MULT;
}

void IntExprMult::buildOpNameString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(" * ");
}

int IntExprMult::getBindingStrength() const
{
	return 21;
}


int IntExprDiv::evaluate(const CvGameObject* pObject) const
{
	const int iDiv = m_pExpr2->evaluate(pObject);
	return iDiv ? m_pExpr1->evaluate(pObject) / iDiv : m_pExpr1->evaluate(pObject);
}

IntExprTypes IntExprDiv::getType() const
{
	return INTEXPR_DIV;
}

void IntExprDiv::buildOpNameString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(" / ");
}

int IntExprDiv::getBindingStrength() const
{
	return 21;
}


IntExprIf::~IntExprIf()
{
	SAFE_DELETE(m_pExprIf);
	SAFE_DELETE(m_pExprThen);
	SAFE_DELETE(m_pExprElse);
}

int IntExprIf::evaluate(const CvGameObject* pObject) const
{
	return m_pExprIf->evaluate(pObject) ? m_pExprThen->evaluate(pObject) : m_pExprElse->evaluate(pObject);
}

void IntExprIf::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	bool bBracketsIf = getBindingStrength() > m_pExprIf->getBindingStrength();
	bool bBracketsThen = getBindingStrength() > m_pExprThen->getBindingStrength();
	bool bBracketsElse = getBindingStrength() > m_pExprElse->getBindingStrength();

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

int IntExprIf::getBindingStrength() const
{
	return 25;
}

void IntExprIf::getCheckSum(uint32_t& iSum) const
{
	m_pExprIf->getCheckSum(iSum);
	m_pExprThen->getCheckSum(iSum);
	m_pExprElse->getCheckSum(iSum);
}


IntExprIntegrateOp::~IntExprIntegrateOp()
{
	SAFE_DELETE(m_pExpr);
}

int IntExprIntegrateOp::evaluate(const CvGameObject* pObject) const
{
	PROFILE_EXTRA_FUNC();
	int iAcc = 0;
	pObject->foreachRelated(m_eType, m_eRelation, bind(getOp(), _1, m_pExpr, &iAcc));
	return iAcc;
}

void IntExprIntegrateOp::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	// TODO: Proper rendering of relations
	m_pExpr->buildDisplayString(szBuffer);
}

int IntExprIntegrateOp::getBindingStrength() const
{
	// TODO: Review when proper rendering of the build string is done
	return m_pExpr->getBindingStrength();
}

void IntExprIntegrateOp::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, (int)getType());
	CheckSum(iSum, (int)m_eRelation);
	CheckSum(iSum, m_iData);
	CheckSum(iSum, (int)m_eType);
	m_pExpr->getCheckSum(iSum);
}


void evalExprIntegrateSum(const CvGameObject* pObject, const IntExpr* pExpr, int* iAcc)
{
	*iAcc = *iAcc + pExpr->evaluate(pObject);
}

IntExprTypes IntExprIntegrateSum::getType() const
{
	return INTEXPR_INTEGRATE_SUM;
}

IntegrateOpFunc IntExprIntegrateSum::getOp() const
{
	return &evalExprIntegrateSum;
}


void evalExprIntegrateAvg(const CvGameObject* pObject, const IntExpr* pExpr, int* iAcc, int* iCount)
{
	*iAcc = *iAcc + pExpr->evaluate(pObject);
	++*iCount;
}

IntExprTypes IntExprIntegrateAvg::getType() const
{
	return INTEXPR_INTEGRATE_AVG;
}

int IntExprIntegrateAvg::evaluate(const CvGameObject* pObject) const
{
	PROFILE_EXTRA_FUNC();
	int iAcc = 0;
	int iCount = 0;
	pObject->foreachRelated(m_eType, m_eRelation, bind(evalExprIntegrateAvg, _1, m_pExpr, &iAcc, &iCount));
	return iCount ? iAcc/iCount : 0;
}

IntegrateOpFunc IntExprIntegrateAvg::getOp() const
{
	return NULL;
}


void evalExprIntegrateCount(const CvGameObject* pObject, const BoolExpr* pExpr, int* iAcc)
{
	if (pExpr->evaluate(pObject))
	{
		++*iAcc;
	}
}

IntExprIntegrateCount::~IntExprIntegrateCount()
{
	SAFE_DELETE(m_pExpr);
}

int IntExprIntegrateCount::evaluate(const CvGameObject* pObject) const
{
	PROFILE_EXTRA_FUNC();
	int iAcc = 0;
	pObject->foreachRelated(m_eType, m_eRelation, bind(evalExprIntegrateCount, _1, m_pExpr, &iAcc));
	return iAcc;
}

void IntExprIntegrateCount::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	// TODO: Proper rendering of relations
	m_pExpr->buildDisplayString(szBuffer);
}

int IntExprIntegrateCount::getBindingStrength() const
{
	// TODO: Review when proper rendering of the build string is done
	return m_pExpr->getBindingStrength();
}

void IntExprIntegrateCount::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, (int)m_eRelation);
	CheckSum(iSum, m_iData);
	CheckSum(iSum, (int)m_eType);
	m_pExpr->getCheckSum(iSum);
}


IntExprRandom::~IntExprRandom()
{
	SAFE_DELETE(m_pExpr);
}

int IntExprRandom::evaluate(const CvGameObject* pObject) const
{
	return GC.getGame().getSorenRandNum(m_pExpr->evaluate(pObject), "Random integer expression");
}

void IntExprRandom::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	szBuffer.append(CvWString::format(L"%c(", gDLL->getSymbolID(RANDOM_CHAR)));
	m_pExpr->buildDisplayString(szBuffer);
	szBuffer.append(L")");
}

int IntExprRandom::getBindingStrength() const
{
	return 100;
}

void IntExprRandom::getCheckSum(uint32_t& iSum) const
{
	m_pExpr->getCheckSum(iSum);
}


IntExprAdapt::~IntExprAdapt()
{
	SAFE_DELETE(m_pExpr);
}

int IntExprAdapt::evaluate(const CvGameObject* pObject) const
{
	return pObject->adaptValueToGame(m_iID, m_pExpr->evaluate(pObject));
}

void IntExprAdapt::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szTextVal;
	szTextVal.Format(L"%d", GC.getGame().getGameObject()->adaptValueToGame(m_iID, m_pExpr->evaluate(GC.getGame().getGameObject())));
	szBuffer.append(szTextVal);
}

int IntExprAdapt::getBindingStrength() const
{
	return 100;
}

void IntExprAdapt::getCheckSum(uint32_t& iSum) const
{
	m_pExpr->getCheckSum(iSum);
	CheckSum(iSum, m_iID);
}


int IntExprPython::evaluate(const CvGameObject* pObject) const
{
	return Cy::call<int>("CvOutcomeInterface", m_szPythonCallback, Cy::Args() << const_cast<CvGameObject*>(pObject));
}

void IntExprPython::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	CvWString szResult = Cy::call<CvWString>("CvOutcomeInterface", m_szPythonCallback, Cy::Args() << false);
	szBuffer.append(szResult);
}

int IntExprPython::getBindingStrength() const
{
	return 100;
}

void IntExprPython::getCheckSum(uint32_t& iSum) const
{
	CheckSumC(iSum, m_szPythonCallback);
}
