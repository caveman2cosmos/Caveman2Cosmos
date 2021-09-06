#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertySource.h
//
//  PURPOSE: Source of generic properties for Civ4 classes
//
//------------------------------------------------------------------------------------------------
#ifndef CV_PROPERTY_SOURCE_H
#define CV_PROPERTY_SOURCE_H

class CvGameObject;
class CvXMLLoadUtility;
class IntExpr;
class PropertySourceContext;

// Base class of which the actual property source classes are derived
class CvPropertySource
{
public:
	CvPropertySource();
	explicit CvPropertySource(PropertyTypes eProperty);
	virtual ~CvPropertySource();

	PropertyTypes getProperty() const;
	GameObjectTypes getObjectType() const;
	RelationTypes getRelation() const;
	int getRelationData() const;

	virtual bool isActive(const CvGameObject* pObject) const;

	virtual PropertySourceTypes getType() const = 0;

	virtual int getSourcePredict(const CvGameObject*, int, PropertySourceContext* = NULL) const = 0;
	virtual int getSourceCorrect(const CvGameObject*, int, int, const PropertySourceContext* = NULL) const = 0;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertySource* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;
protected:
	PropertyTypes m_eProperty;
	GameObjectTypes m_eObjectType;
	RelationTypes m_eRelation;
	int m_iRelationData;
	const BoolExpr* m_pExprActive;
};


class CvPropertySourceConstant : public CvPropertySource
{
public:
	CvPropertySourceConstant();
	explicit CvPropertySourceConstant(PropertyTypes eProperty);
	CvPropertySourceConstant(PropertyTypes eProperty, const IntExpr* pAmountPerTurn);

	int getAmountPerTurn(const CvGameObject* pObject) const;
	const IntExpr* getAmountPerTurnExpr() const { return m_pAmountPerTurn; }

	virtual PropertySourceTypes getType() const { return PROPERTYSOURCE_CONSTANT; }

	virtual int getSourcePredict(const CvGameObject* pObject, int iCurrentAmount, PropertySourceContext* pContext = NULL) const;
	virtual int getSourceCorrect(const CvGameObject* pObject, int iCurrentAmount, int iPredictedAmount, const PropertySourceContext* pContext = NULL) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertySource* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	const IntExpr* m_pAmountPerTurn;
};

class CvPropertySourceConstantLimited : public CvPropertySource
{
public:
	CvPropertySourceConstantLimited();
	explicit CvPropertySourceConstantLimited(PropertyTypes eProperty);
	CvPropertySourceConstantLimited(PropertyTypes eProperty, int iAmountPerTurn, int iLimit);

	int getAmountPerTurn() const { return m_iAmountPerTurn; }
	int getLimit() const { return m_iLimit; }

	virtual PropertySourceTypes getType() const { return PROPERTYSOURCE_CONSTANT_LIMITED; }

	virtual int getSourcePredict(const CvGameObject* pObject, int iCurrentAmount, PropertySourceContext* pContext = NULL) const;
	virtual int getSourceCorrect(const CvGameObject* pObject, int iCurrentAmount, int iPredictedAmount, const PropertySourceContext* pContext = NULL) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertySource* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iAmountPerTurn;
	int m_iLimit;
};

class CvPropertySourceDecay : public CvPropertySource
{
public:
	CvPropertySourceDecay();
	explicit CvPropertySourceDecay(PropertyTypes eProperty);
	CvPropertySourceDecay(PropertyTypes eProperty, int iPercent, int iNoDecayAmount);

	int getPercent() const { return m_iPercent; }
	int getNoDecayAmount() const { return m_iNoDecayAmount; }

	virtual PropertySourceTypes getType() const { return PROPERTYSOURCE_DECAY; }

	virtual bool isActive(const CvGameObject* pObject) const;

	virtual int getSourcePredict(const CvGameObject* pObject, int iCurrentAmount, PropertySourceContext* pContext = NULL) const;
	virtual int getSourceCorrect(const CvGameObject* pObject, int iCurrentAmount, int iPredictedAmount, const PropertySourceContext* pContext = NULL) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertySource* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iPercent;
	int m_iNoDecayAmount;
};

class CvPropertySourceAttributeConstant : public CvPropertySource
{
public:
	CvPropertySourceAttributeConstant();
	explicit CvPropertySourceAttributeConstant(PropertyTypes eProperty);
	CvPropertySourceAttributeConstant(PropertyTypes eProperty, AttributeTypes eAttribute, int iAmountPerTurn);

	AttributeTypes getAttribute() const { return m_eAttribute; }
	int getAmountPerTurn() const { return m_iAmountPerTurn; }

	virtual PropertySourceTypes getType() const { return PROPERTYSOURCE_ATTRIBUTE_CONSTANT; }

	virtual int getSourcePredict(const CvGameObject* pObject, int iCurrentAmount, PropertySourceContext* pContext = NULL) const;
	virtual int getSourceCorrect(const CvGameObject* pObject, int iCurrentAmount, int iPredictedAmount, const PropertySourceContext* pContext = NULL) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertySource* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	AttributeTypes m_eAttribute;
	int m_iAmountPerTurn;
};

#endif