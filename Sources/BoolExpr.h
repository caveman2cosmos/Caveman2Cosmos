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
	const CvGameObject* pObject;
	GOMTypes GOM;
	int id;
	bool bHas;
};

enum BoolExprChange
{
	NO_BOOLEXPR_CHANGE = -1,
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
	virtual bool evaluate(const CvGameObject*) const { return false; }
	virtual BoolExprChange evaluateChange(const CvGameObject*, const std::vector<GOMOverride>&) const { return NO_BOOLEXPR_CHANGE; }
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>&) const { return false; }
	static const BoolExpr* read(CvXMLLoadUtility* pXML);
	virtual void getCheckSum(uint32_t&) const { }
	virtual void buildDisplayString(CvWStringBuffer&) const { }
	virtual int getBindingStrength() const { return 0; } // How strong the operator binds in language so brackets can be placed appropriately
	virtual BoolExprTypes getType() const { return NO_BOOLEXPR; }
};

class BoolExprConstant : public BoolExpr
{
public:
	BoolExprConstant(bool bValue = false) : m_bValue(bValue) {}
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
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
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
	void readContent(CvXMLLoadUtility* pXML);
	virtual void getCheckSum(uint32_t& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;

	virtual BoolExprTypes getType() const	{ return BOOLEXPR_HAS; }
	GOMTypes getGOMType() const				{ return m_eGOM; }
	int getID() const						{ return m_iID; }
protected:
	GOMTypes m_eGOM;
	int m_iID;
};

class BoolExprIs : public BoolExpr
{
public:
	BoolExprIs(TagTypes eTag = NO_TAG) : m_eTag(eTag) {}
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
	virtual void getCheckSum(uint32_t& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	const TagTypes m_eTag;
};

class BoolExprNot : public BoolExpr
{
public:
	BoolExprNot(const BoolExpr* pExpr = NULL) : m_pExpr(pExpr) {}
	virtual ~BoolExprNot();
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
	virtual void getCheckSum(uint32_t& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	const BoolExpr* m_pExpr;
};

class BoolExprAnd : public BoolExpr
{
public:
	BoolExprAnd(const BoolExpr* pExpr1 = NULL, const BoolExpr* pExpr2 = NULL) : m_pExpr1(pExpr1), m_pExpr2(pExpr2) {}
	virtual ~BoolExprAnd();
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
	virtual void getCheckSum(uint32_t& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;

	virtual BoolExprTypes getType() const	{ return BOOLEXPR_AND; }
	const BoolExpr* getFirstExpr() const	{ return m_pExpr1; }
	const BoolExpr* getSecondExpr() const	{ return m_pExpr2; }
protected:
	const BoolExpr* m_pExpr1;
	const BoolExpr* m_pExpr2;
};

class BoolExprOr : public BoolExpr
{
public:
	BoolExprOr(const BoolExpr* pExpr1 = NULL, const BoolExpr* pExpr2 = NULL) : m_pExpr1(pExpr1), m_pExpr2(pExpr2) {}
	virtual ~BoolExprOr();
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
	virtual void getCheckSum(uint32_t& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
	
	virtual BoolExprTypes getType() const	{ return BOOLEXPR_OR; }
	const BoolExpr* getFirstExpr() const	{ return m_pExpr1; }
	const BoolExpr* getSecondExpr() const	{ return m_pExpr2; }
protected:
	const BoolExpr* m_pExpr1;
	const BoolExpr* m_pExpr2;
};

class BoolExprBEqual : public BoolExpr
{
public:
	BoolExprBEqual(const BoolExpr* pExpr1 = NULL, const BoolExpr* pExpr2 = NULL) : m_pExpr1(pExpr1), m_pExpr2(pExpr2) {}
	virtual ~BoolExprBEqual();
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
	virtual void getCheckSum(uint32_t& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	const BoolExpr* m_pExpr1;
	const BoolExpr* m_pExpr2;
};

class BoolExprIf : public BoolExpr
{
public:
	BoolExprIf(const BoolExpr* pExprIf = NULL, const BoolExpr* pExprThen = NULL, const BoolExpr* pExprElse = NULL) : m_pExprIf(pExprIf), m_pExprThen(pExprThen), m_pExprElse(pExprElse) {}
	virtual ~BoolExprIf();
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
	virtual void getCheckSum(uint32_t& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	const BoolExpr* m_pExprIf;
	const BoolExpr* m_pExprThen;
	const BoolExpr* m_pExprElse;
};

class BoolExprIntegrateOr : public BoolExpr
{
public:
	BoolExprIntegrateOr(const BoolExpr* pExpr = NULL, RelationTypes eRelation = NO_RELATION, int iData = -1, GameObjectTypes eType = NO_GAMEOBJECT) : m_pExpr(pExpr), m_eRelation(eRelation), m_iData(iData), m_eType(eType) {}
	virtual ~BoolExprIntegrateOr();
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
	virtual void getCheckSum(uint32_t& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	const BoolExpr* m_pExpr;
	const RelationTypes m_eRelation;
	const int m_iData;
	const GameObjectTypes m_eType;
};

class BoolExprComp : public BoolExpr
{
public:
	BoolExprComp(const IntExpr* pExpr1 = NULL, const IntExpr* pExpr2 = NULL) : m_pExpr1(pExpr1), m_pExpr2(pExpr2) {}
	virtual ~BoolExprComp();
	virtual BoolExprChange evaluateChange(const CvGameObject* pObject, const std::vector<GOMOverride>& overrides) const;
	virtual bool getInvolvesGOM(const std::vector<GOMQuery>& queries) const;
	virtual void getCheckSum(uint32_t& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual BoolExprTypes getType() const = 0;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const = 0;
	virtual int getBindingStrength() const;
protected:
	const IntExpr* m_pExpr1;
	const IntExpr* m_pExpr2;
};

class BoolExprGreater : public BoolExprComp
{
public:
	BoolExprGreater(const IntExpr* pExpr1 = NULL, const IntExpr* pExpr2 = NULL) : BoolExprComp(pExpr1, pExpr2) {}
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
};

class BoolExprGreaterEqual : public BoolExprComp
{
public:
	BoolExprGreaterEqual(const IntExpr* pExpr1 = NULL, const IntExpr* pExpr2 = NULL) : BoolExprComp(pExpr1, pExpr2) {}
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
};

class BoolExprEqual : public BoolExprComp
{
public:
	BoolExprEqual(const IntExpr* pExpr1 = NULL, const IntExpr* pExpr2 = NULL) : BoolExprComp(pExpr1, pExpr2) {}
	virtual bool evaluate(const CvGameObject* pObject) const;
	virtual BoolExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
};

#endif