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

struct GOMOverride
{
	CvGameObject* pObject;
	GOMTypes GOM;
	int id;
	bool bHas;
};

enum BoolExprChange
{
	BOOLEXPR_CHANGE_REMAINS_TRUE,
	BOOLEXPR_CHANGE_REMAINS_FALSE,
	BOOLEXPR_CHANGE_BECOMES_TRUE,
	BOOLEXPR_CHANGE_BECOMES_FALSE
};

struct GOMQuery
{
	GOMTypes GOM;
	int id;
};

class BoolExpr
{
public:
	virtual ~BoolExpr();
	virtual bool evaluate(const CvGameObject* pObject) const = 0;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const = 0;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd) = 0;
	static BoolExpr* read(CvXMLLoadUtility* pXML);
	virtual void getCheckSum(uint32_t& iSum) const = 0;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const = 0;
	virtual int getBindingStrength() const = 0; // How strong the operator binds in language so brackets can be placed appropriately
};

class BoolExprConstant : public BoolExpr
{
public:
	BoolExprConstant(bool bValue = false) : m_bValue(bValue) {}
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	void readConstant(CvXMLLoadUtility* pXML);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	void readContent(CvXMLLoadUtility* pXML);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, GOMOverride* pBegin, GOMOverride* pEnd) const;
	virtual bool getInvolvesGOM(GOMQuery* pBegin, GOMQuery* pEnd);
	virtual void getCheckSum(uint32_t& iSum) const;
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
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
};

class BoolExprGreaterEqual : public BoolExprComp
{
public:
	BoolExprGreaterEqual(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : BoolExprComp(pExpr1, pExpr2) {}
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
};

class BoolExprEqual : public BoolExprComp
{
public:
	BoolExprEqual(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : BoolExprComp(pExpr1, pExpr2) {}
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
};

#endif