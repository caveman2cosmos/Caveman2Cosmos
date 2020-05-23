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

// Base class of which the actual property propagator classes are derived
class CvPropertyPropagator
{
public:
	CvPropertyPropagator();
	CvPropertyPropagator(PropertyTypes eProperty);
	virtual ~CvPropertyPropagator();
	PropertyTypes getProperty() const;
	void setProperty(PropertyTypes eProperty);
	GameObjectTypes getObjectType() const;
	void setObjectType(GameObjectTypes eObjectType);
	RelationTypes getRelation() const;
	void setRelation(RelationTypes eRelation);
	int getRelationData() const;
	void setRelationData(int iRelationData);
	RelationTypes getTargetRelation() const;
	void setTargetRelation(RelationTypes eTargetRelation);
	int getTargetRelationData() const;
	void setTargetRelationData(int iRelationData);
	GameObjectTypes getTargetObjectType() const;
	void setTargetObjectType(GameObjectTypes eObjectType);

	bool isActive(CvGameObject* pObject);
	void getTargetObjects(CvGameObject* pObject, std::vector<CvGameObject*>& apGameObjects);

	virtual PropertyPropagatorTypes getType() = 0;

	virtual void getPredict(std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict) = 0;
	virtual void getCorrect(std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect) = 0;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyPropagator* pProp, CvXMLLoadUtility* pXML );

	virtual void getCheckSum(unsigned int& iSum);
protected:
	PropertyTypes m_eProperty;
	GameObjectTypes m_eObjectType;
	RelationTypes m_eRelation;
	int m_iRelationData;
	RelationTypes m_eTargetRelation;
	int m_iTargetRelationData;
	GameObjectTypes m_eTargetObjectType;
	BoolExpr* m_pExprActive;
	BoolExpr* m_pExprTargetCondition;
};


class CvPropertyPropagatorSpread : public CvPropertyPropagator
{
public:
	CvPropertyPropagatorSpread();
	explicit CvPropertyPropagatorSpread(PropertyTypes eProperty);
	CvPropertyPropagatorSpread(PropertyTypes eProperty, int iPercent);

	virtual PropertyPropagatorTypes getType();

	int getPercent();

	//virtual bool isActive(CvGameObject* pObject);

	virtual void getPredict(std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict);
	virtual void getCorrect(std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect);

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyPropagator* pProp, CvXMLLoadUtility* pXML );

	virtual void getCheckSum(unsigned int& iSum);

protected:
	int m_iPercent;
};

class CvPropertyPropagatorGather : public CvPropertyPropagator
{
public:
	CvPropertyPropagatorGather();
	explicit CvPropertyPropagatorGather(PropertyTypes eProperty);
	CvPropertyPropagatorGather(PropertyTypes eProperty, int iAmountPerTurn);

	virtual PropertyPropagatorTypes getType();

	int getAmountPerTurn();

	virtual void getPredict(std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict);
	virtual void getCorrect(std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect);

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyPropagator* pProp, CvXMLLoadUtility* pXML );

	virtual void getCheckSum(unsigned int& iSum);

protected:
	int m_iAmountPerTurn;
};

class CvPropertyPropagatorDiffuse : public CvPropertyPropagator
{
public:
	CvPropertyPropagatorDiffuse();
	explicit CvPropertyPropagatorDiffuse(PropertyTypes eProperty);
	CvPropertyPropagatorDiffuse(PropertyTypes eProperty, int iPercent);

	virtual PropertyPropagatorTypes getType();

	int getPercent();

	//virtual bool isActive(CvGameObject* pObject);

	virtual void getPredict(std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredict);
	virtual void getCorrect(std::vector<int>& aiCurrentAmount, std::vector<int>& aiPredictedAmount, std::vector<int>& aiCorrect);

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyPropagator* pProp, CvXMLLoadUtility* pXML );

	virtual void getCheckSum(unsigned int& iSum);

protected:
	int m_iPercent;
};

#endif