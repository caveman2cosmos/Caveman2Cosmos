#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    BoolExpr.h
//
//  PURPOSE: Boolean expressions for Civ4 classes
//
//------------------------------------------------------------------------------------------------
#ifndef BOOLEXPR_H
#define BOOLEXPR_H

class CvGameObject;
class IntExpr;

enum BoolExprTypes
{
	NO_BOOLEXPR = -1,
	BOOLEXPR_CONSTANT,
	BOOLEXPR_HAS,
	BOOLEXPR_IS,
	BOOLEXPR_NOT,
	BOOLEXPR_AND,
	BOOLEXPR_OR,
	BOOLEXPR_BEQUAL,
	BOOLEXPR_IF,
	BOOLEXPR_INTEGRATE_OR,
	BOOLEXPR_GREATER,
	BOOLEXPR_GREATER_EQUAL,
	BOOLEXPR_EQUAL
};

class BoolExpr
{
public:
	virtual ~BoolExpr();
	virtual bool evaluate(CvGameObject* pObject) = 0;
	static BoolExpr* read(CvXMLLoadUtility* pXML);
	virtual void getCheckSum(unsigned int& iSum) const = 0;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const = 0;
	virtual int getBindingStrength() const = 0; // How strong the operator binds in language so brackets can be placed appropriately
};

class BoolExprConstant : public BoolExpr
{
public:
	BoolExprConstant(bool bValue = false) : m_bValue(bValue) {}
	virtual bool evaluate(CvGameObject* pObject);
	void readConstant(CvXMLLoadUtility* pXML);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	bool m_bValue;
};

class BoolExprHas : public BoolExpr
{
public:
	BoolExprHas(GOMTypes eGOM = NO_GOM, int iID = -1) : m_eGOM(eGOM), m_iID(iID) {}
	virtual ~BoolExprHas();
	virtual bool evaluate(CvGameObject* pObject);
	void readContent(CvXMLLoadUtility* pXML);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	GOMTypes m_eGOM;
	int m_iID;
};

class BoolExprIs : public BoolExpr
{
public:
	BoolExprIs(TagTypes eTag = NO_TAG) : m_eTag(eTag) {}
	virtual bool evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	TagTypes m_eTag;
};

class BoolExprNot : public BoolExpr
{
public:
	BoolExprNot(BoolExpr* pExpr = NULL) : m_pExpr(pExpr) {}
	virtual ~BoolExprNot();
	virtual bool evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	BoolExpr* m_pExpr;
};

class BoolExprAnd : public BoolExpr
{
public:
	BoolExprAnd(BoolExpr* pExpr1 = NULL, BoolExpr* pExpr2 = NULL) : m_pExpr1(pExpr1), m_pExpr2(pExpr2) {}
	virtual ~BoolExprAnd();
	virtual bool evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	BoolExpr* m_pExpr1;
	BoolExpr* m_pExpr2;
};

class BoolExprOr : public BoolExpr
{
public:
	BoolExprOr(BoolExpr* pExpr1 = NULL, BoolExpr* pExpr2 = NULL) : m_pExpr1(pExpr1), m_pExpr2(pExpr2) {}
	virtual ~BoolExprOr();
	virtual bool evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	BoolExpr* m_pExpr1;
	BoolExpr* m_pExpr2;
};

class BoolExprBEqual : public BoolExpr
{
public:
	BoolExprBEqual(BoolExpr* pExpr1 = NULL, BoolExpr* pExpr2 = NULL) : m_pExpr1(pExpr1), m_pExpr2(pExpr2) {}
	virtual ~BoolExprBEqual();
	virtual bool evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	BoolExpr* m_pExpr1;
	BoolExpr* m_pExpr2;
};

class BoolExprIf : public BoolExpr
{
public:
	BoolExprIf(BoolExpr* pExprIf = NULL, BoolExpr* pExprThen = NULL, BoolExpr* pExprElse = NULL) : m_pExprIf(pExprIf), m_pExprThen(pExprThen), m_pExprElse(pExprElse) {}
	virtual ~BoolExprIf();
	virtual bool evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	BoolExpr* m_pExprIf;
	BoolExpr* m_pExprThen;
	BoolExpr* m_pExprElse;
};

class BoolExprIntegrateOr : public BoolExpr
{
public:
	BoolExprIntegrateOr(BoolExpr* pExpr = NULL, RelationTypes eRelation = NO_RELATION, int iData = -1, GameObjectTypes eType = NO_GAMEOBJECT) : m_pExpr(pExpr), m_eRelation(eRelation), m_iData(iData), m_eType(eType) {}
	virtual ~BoolExprIntegrateOr();
	virtual bool evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	BoolExpr* m_pExpr;
	RelationTypes m_eRelation;
	int m_iData;
	GameObjectTypes m_eType;
};

class BoolExprComp : public BoolExpr
{
public:
	BoolExprComp(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : m_pExpr1(pExpr1), m_pExpr2(pExpr2) {}
	virtual ~BoolExprComp();
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual BoolExprTypes getType() const = 0;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const = 0;
	virtual int getBindingStrength() const;
protected:
	IntExpr* m_pExpr1;
	IntExpr* m_pExpr2;
};

class BoolExprGreater : public BoolExprComp
{
public:
	BoolExprGreater(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : BoolExprComp(pExpr1, pExpr2) {}
	virtual bool evaluate(CvGameObject* pObject);
	virtual BoolExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
};

class BoolExprGreaterEqual : public BoolExprComp
{
public:
	BoolExprGreaterEqual(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : BoolExprComp(pExpr1, pExpr2) {}
	virtual bool evaluate(CvGameObject* pObject);
	virtual BoolExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
};

class BoolExprEqual : public BoolExprComp
{
public:
	BoolExprEqual(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : BoolExprComp(pExpr1, pExpr2) {}
	virtual bool evaluate(CvGameObject* pObject);
	virtual BoolExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
};

#endif