#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertyPropagator.h
//
//  PURPOSE: Propagator of generic properties for Civ4 classes
//
//------------------------------------------------------------------------------------------------
#ifndef CV_PROPERTY_PROPAGATOR_H
#define CV_PROPERTY_PROPAGATOR_H

class CvGameObject;
class CvXMLLoadUtility;

// Base class of which the actual property propagator classes are derived
class CvPropertyPropagator
{
public:
	CvPropertyPropagator();
	CvPropertyPropagator(PropertyTypes eProperty);
	virtual ~CvPropertyPropagator();

	PropertyTypes getProperty() const;
	GameObjectTypes getObjectType() const;
	RelationTypes getRelation() const;
	int getRelationData() const;
	RelationTypes getTargetRelation() const;
	int getTargetRelationData() const;
	GameObjectTypes getTargetObjectType() const;

	bool isActive(const CvGameObject* pObject) const;
	void getTargetObjects(const CvGameObject* pObject, std::vector<const CvGameObject*>& apGameObjects) const;

	virtual PropertyPropagatorTypes getType() const = 0;

	virtual void getPredict(const std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict) const = 0;
	virtual void getCorrect(const std::vector<int>& aiCurrentAmount, const std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect) const = 0;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyPropagator* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;
protected:
	PropertyTypes m_eProperty;
	GameObjectTypes m_eObjectType;
	RelationTypes m_eRelation;
	int m_iRelationData;
	RelationTypes m_eTargetRelation;
	int m_iTargetRelationData;
	GameObjectTypes m_eTargetObjectType;
	const BoolExpr* m_pExprActive;
	const BoolExpr* m_pExprTargetCondition;
};


class CvPropertyPropagatorSpread : public CvPropertyPropagator
{
public:
	CvPropertyPropagatorSpread();
	explicit CvPropertyPropagatorSpread(PropertyTypes eProperty);
	CvPropertyPropagatorSpread(PropertyTypes eProperty, int iPercent);

	virtual PropertyPropagatorTypes getType() const;

	int getPercent() const;

	//virtual bool isActive(CvGameObject* pObject);

	virtual void getPredict(const std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict) const;
	virtual void getCorrect(const std::vector<int>& aiCurrentAmount, const std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyPropagator* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iPercent;
};

class CvPropertyPropagatorGather : public CvPropertyPropagator
{
public:
	CvPropertyPropagatorGather();
	explicit CvPropertyPropagatorGather(PropertyTypes eProperty);
	CvPropertyPropagatorGather(PropertyTypes eProperty, int iAmountPerTurn);

	virtual PropertyPropagatorTypes getType() const;

	int getAmountPerTurn() const;

	virtual void getPredict(const std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict) const;
	virtual void getCorrect(const std::vector<int>& aiCurrentAmount, const std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyPropagator* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iAmountPerTurn;
};

class CvPropertyPropagatorDiffuse : public CvPropertyPropagator
{
public:
	CvPropertyPropagatorDiffuse();
	explicit CvPropertyPropagatorDiffuse(PropertyTypes eProperty);
	CvPropertyPropagatorDiffuse(PropertyTypes eProperty, int iPercent);

	virtual PropertyPropagatorTypes getType() const;

	int getPercent() const;

	//virtual bool isActive(CvGameObject* pObject);

	virtual void getPredict(const std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict) const;
	virtual void getCorrect(const std::vector<int>& aiCurrentAmount, const std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyPropagator* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iPercent;
};

#endif