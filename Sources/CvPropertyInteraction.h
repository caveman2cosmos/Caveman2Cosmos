#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertyInteraction.h
//
//  PURPOSE: Interaction of generic properties for Civ4 classes
//
//------------------------------------------------------------------------------------------------
#ifndef CV_PROPERTY_INTERACTION_H
#define CV_PROPERTY_INTERACTION_H

class CvGameObject;
class CvXMLLoadUtility;
class BoolExpr;

// Base class of which the actual property interaction classes are derived
class CvPropertyInteraction
{
public:
	CvPropertyInteraction();
	CvPropertyInteraction(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty);
	virtual ~CvPropertyInteraction();

	PropertyTypes getSourceProperty() const;
	PropertyTypes getTargetProperty() const;
	GameObjectTypes getObjectType() const;
	RelationTypes getRelation() const;
	int getRelationData() const;

	bool isActive(const CvGameObject* pObject) const;

	virtual PropertyInteractionTypes getType() const = 0;

	virtual std::pair<int,int> getPredict(int iCurrentAmountSource, int iCurrentAmountTarget) const = 0;
	virtual std::pair<int,int> getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget) const = 0;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyInteraction* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;
protected:
	PropertyTypes m_eSourceProperty;
	PropertyTypes m_eTargetProperty;
	GameObjectTypes m_eObjectType;
	RelationTypes m_eRelation;
	int m_iRelationData;
	const BoolExpr* m_pExprActive;
};


class CvPropertyInteractionConvertConstant : public CvPropertyInteraction
{
public:
	CvPropertyInteractionConvertConstant();
	CvPropertyInteractionConvertConstant(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty);
	CvPropertyInteractionConvertConstant(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty, int iAmountPerTurn);

	virtual PropertyInteractionTypes getType() const;

	int getAmountPerTurn() const;

	virtual std::pair<int,int> getPredict(int iCurrentAmountSource, int iCurrentAmountTarget) const;
	virtual std::pair<int,int> getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyInteraction* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iAmountPerTurn;
};


class CvPropertyInteractionInhibitedGrowth : public CvPropertyInteraction
{
public:
	CvPropertyInteractionInhibitedGrowth();
	CvPropertyInteractionInhibitedGrowth(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty);
	CvPropertyInteractionInhibitedGrowth(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty, int iGrowthPercent, int iInhibitionPercent);

	virtual PropertyInteractionTypes getType() const;

	int getGrowthPercent() const;
	int getInhibitionPercent() const;

	virtual std::pair<int,int> getPredict(int iCurrentAmountSource, int iCurrentAmountTarget) const;
	virtual std::pair<int,int> getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyInteraction* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iGrowthPercent;
	int m_iInhibitionPercent;
};


class CvPropertyInteractionConvertPercent : public CvPropertyInteraction
{
public:
	CvPropertyInteractionConvertPercent();
	CvPropertyInteractionConvertPercent(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty);
	CvPropertyInteractionConvertPercent(PropertyTypes eSourceProperty, PropertyTypes eTargetProperty, int iPercent, int iNoConvertAmount);

	virtual PropertyInteractionTypes getType() const;

	int getPercent() const;
	int getNoConvertAmount() const;

	virtual std::pair<int,int> getPredict(int iCurrentAmountSource, int iCurrentAmountTarget) const;
	virtual std::pair<int,int> getCorrect(int iCurrentAmountSource, int iCurrentAmountTarget, int iPredictedAmountSource, int iPredictedAmountTarget) const;

	virtual void buildDisplayString(CvWStringBuffer& szBuffer) const;

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual void copyNonDefaults(CvPropertyInteraction* pProp);

	virtual void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iPercent;
	int m_iNoConvertAmount;
};

#endif