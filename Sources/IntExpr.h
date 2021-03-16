#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    IntExpr.h
//
//  PURPOSE: Integer expressions for Civ4 classes
//
//------------------------------------------------------------------------------------------------
#ifndef INTEXPR_H
#define INTEXPR_H

class CvGameObject;
class CvXMLLoadUtility;
class BoolExpr;

enum IntExprTypes
{
	NO_INTEXPR = -1,
	INTEXPR_CONSTANT,
	INTEXPR_ATTRIBUTE,
	INTEXPR_PROPERTY,
	INTEXPR_PLUS,
	INTEXPR_MINUS,
	INTEXPR_MULT,
	INTEXPR_DIV,
	INTEXPR_IF,
	INTEXPR_INTEGRATE_SUM,
	INTEXPR_INTEGRATE_AVG,
	INTEXPR_INTEGRATE_COUNT,
	INTEXPR_RANDOM,
	INTEXPR_PYTHON,
	INTEXPR_ADAPT
};

class IntExpr
{
public:
	virtual ~IntExpr() = 0;
	virtual int evaluate(CvGameObject* pObject) = 0;
	static IntExpr* read(CvXMLLoadUtility* pXML);
	virtual void getCheckSum(unsigned int& iSum) const = 0;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const = 0;
	virtual int getBindingStrength() const = 0; // How strong the operator binds in language so brackets can be placed appropriately
	virtual bool isConstantZero() const;
};

class IntExprConstant : public IntExpr
{
public:
	IntExprConstant(int iValue = 0) : m_iValue(iValue) {}
	virtual int evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
	virtual bool isConstantZero() const;
protected:
	int m_iValue;
};

class IntExprAttribute : public IntExpr
{
public:
	IntExprAttribute(AttributeTypes eAttribute = NO_ATTRIBUTE) : m_eAttribute(eAttribute) {}
	virtual int evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	AttributeTypes m_eAttribute;
};

class IntExprProperty : public IntExpr
{
public:
	IntExprProperty(PropertyTypes eProperty = NO_PROPERTY) : m_eProperty(eProperty) {}
	virtual int evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	PropertyTypes m_eProperty;
};

class IntExprOp : public IntExpr
{
public:
	IntExprOp(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : m_pExpr1(pExpr1), m_pExpr2(pExpr2) {}
	virtual ~IntExprOp();
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const = 0;
	virtual IntExprTypes getType() const = 0;
protected:
	IntExpr* m_pExpr1;
	IntExpr* m_pExpr2;
};

class IntExprPlus : public IntExprOp
{
public:
	IntExprPlus(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : IntExprOp(pExpr1, pExpr2) {}
	virtual int evaluate(CvGameObject* pObject);
	virtual IntExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
};

class IntExprMinus : public IntExprOp
{
public:
	IntExprMinus(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : IntExprOp(pExpr1, pExpr2) {}
	virtual int evaluate(CvGameObject* pObject);
	virtual IntExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
};

class IntExprMult : public IntExprOp
{
public:
	IntExprMult(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : IntExprOp(pExpr1, pExpr2) {}
	virtual int evaluate(CvGameObject* pObject);
	virtual IntExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
};

class IntExprDiv : public IntExprOp
{
public:
	IntExprDiv(IntExpr* pExpr1 = NULL, IntExpr* pExpr2 = NULL) : IntExprOp(pExpr1, pExpr2) {}
	virtual int evaluate(CvGameObject* pObject);
	virtual IntExprTypes getType() const;
	virtual void buildOpNameString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
};

class IntExprIf : public IntExpr
{
public:
	IntExprIf(BoolExpr* pExprIf = NULL, IntExpr* pExprThen = NULL, IntExpr* pExprElse = NULL) : m_pExprIf(pExprIf), m_pExprThen(pExprThen), m_pExprElse(pExprElse) {}
	virtual ~IntExprIf();
	virtual int evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	BoolExpr* m_pExprIf;
	IntExpr* m_pExprThen;
	IntExpr* m_pExprElse;
};

typedef void (*IntegrateOpFunc)(CvGameObject*, IntExpr*, int*);

class IntExprIntegrateOp : public IntExpr
{
public:
	IntExprIntegrateOp(IntExpr* pExpr = NULL, RelationTypes eRelation = NO_RELATION, int iData = -1, GameObjectTypes eType = NO_GAMEOBJECT) : m_pExpr(pExpr), m_eRelation(eRelation), m_iData(iData), m_eType(eType) {}
	virtual ~IntExprIntegrateOp();
	virtual int evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
	virtual IntExprTypes getType() const = 0;
	virtual IntegrateOpFunc getOp() = 0;
protected:
	IntExpr* m_pExpr;
	RelationTypes m_eRelation;
	int m_iData;
	GameObjectTypes m_eType;
};

class IntExprIntegrateSum : public IntExprIntegrateOp
{
public:
	IntExprIntegrateSum(IntExpr* pExpr = NULL, RelationTypes eRelation = NO_RELATION, int iData = -1, GameObjectTypes eType = NO_GAMEOBJECT) : IntExprIntegrateOp(pExpr, eRelation, iData, eType) {}
	virtual IntExprTypes getType() const;
	virtual IntegrateOpFunc getOp();
};

class IntExprIntegrateAvg : public IntExprIntegrateOp
{
public:
	IntExprIntegrateAvg(IntExpr* pExpr = NULL, RelationTypes eRelation = NO_RELATION, int iData = -1, GameObjectTypes eType = NO_GAMEOBJECT) : IntExprIntegrateOp(pExpr, eRelation, iData, eType) {}
	virtual IntExprTypes getType() const;
	virtual int evaluate(CvGameObject* pObject);
	virtual IntegrateOpFunc getOp();
};

class IntExprIntegrateCount : public IntExpr
{
public:
	IntExprIntegrateCount(BoolExpr* pExpr = NULL, RelationTypes eRelation = NO_RELATION, int iData = -1, GameObjectTypes eType = NO_GAMEOBJECT) : m_pExpr(pExpr), m_eRelation(eRelation), m_iData(iData), m_eType(eType) {}
	virtual ~IntExprIntegrateCount();
	virtual int evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	BoolExpr* m_pExpr;
	RelationTypes m_eRelation;
	int m_iData;
	GameObjectTypes m_eType;
};

class IntExprRandom : public IntExpr
{
public:
	IntExprRandom(IntExpr* pExpr = NULL) : m_pExpr(pExpr) {}
	virtual ~IntExprRandom();
	virtual int evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	IntExpr* m_pExpr;
};

class IntExprPython : public IntExpr
{
public:
	explicit IntExprPython(CvString szPythonCallback) : m_szPythonCallback(szPythonCallback) {}
	virtual int evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	CvString m_szPythonCallback;
};

class IntExprAdapt : public IntExpr
{
public:
	IntExprAdapt(IntExpr* pExpr = NULL, int iID = 0) : m_pExpr(pExpr), m_iID(iID) {}
	virtual ~IntExprAdapt();
	virtual int evaluate(CvGameObject* pObject);
	virtual void getCheckSum(unsigned int& iSum) const;
	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;
	virtual int getBindingStrength() const;
protected:
	IntExpr* m_pExpr;
	int m_iID;
};


#endif