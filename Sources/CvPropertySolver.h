#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertySolver.h
//
//  PURPOSE: Singleton class for solving the system of property manipulators
//
//------------------------------------------------------------------------------------------------
#ifndef CV_PROPERTY_SOLVER_H
#define CV_PROPERTY_SOLVER_H

#include "CvPropertySource.h"
#include "CvPropertyInteraction.h"
#include "CvPropertyPropagator.h"

typedef std::map<const CvGameObject*, CvProperties> PropertySolverMapType;

class PropertySolverMap
{
public:
	int getPredictValue(const CvGameObject* pObject, PropertyTypes eProperty);
	void addChange(const CvGameObject* pObject, PropertyTypes eProperty, int iChange);

	void computePredictValues();
	void applyChanges();

protected:
	PropertySolverMapType m_mapProperties;
	PropertySolverMapType m_mapPropertyChanges;
};

class CvPropertySolver;

class PropertySourceContext
{
public:
	PropertySourceContext(CvPropertySource* pSource, const CvGameObject* pObject);
	CvPropertySource* getSource() const;
	const CvGameObject* getObject() const;
	int getData1() const;
	int getData2() const;
	void setData1(int iData);
	void setData2(int iData);

	void doPredict(CvPropertySolver* pSolver);
	void doCorrect(CvPropertySolver* pSolver);

protected:
	CvPropertySource* m_pSource;
	const CvGameObject* m_pObject;
	int m_iCurrentAmount;
	int m_iData1;
	int m_iData2;
};

class PropertyInteractionContext
{
public:
	PropertyInteractionContext(CvPropertyInteraction* pInteraction, const CvGameObject* pObject);
	CvPropertyInteraction* getInteraction() const;
	const CvGameObject* getObject() const;

	void doPredict(CvPropertySolver* pSolver);
	void doCorrect(CvPropertySolver* pSolver);

protected:
	CvPropertyInteraction* m_pInteraction;
	const CvGameObject* m_pObject;
	int m_iCurrentAmountSource;
	int m_iCurrentAmountTarget;
};

class PropertyPropagatorContext
{
public:
	PropertyPropagatorContext(CvPropertyPropagator* pPropagator, const CvGameObject* pObject);
	CvPropertyPropagator* getPropagator() const;
	const CvGameObject* getObject() const;
	std::vector<const CvGameObject*>* getTargetObjects();

	void doPredict(CvPropertySolver* pSolver);
	void doCorrect(CvPropertySolver* pSolver);

protected:
	CvPropertyPropagator* m_pPropagator;
	const CvGameObject* m_pObject;
	std::vector<const CvGameObject*> m_apTargetObjects;
	std::vector<int> m_aiCurrentAmount;
};

class CvPropertySolver
{
public:
	void doTurn();

	void instantiateSource(const CvGameObject* pObject, CvPropertySource* pSource);
	void instantiateInteraction(const CvGameObject* pObject, CvPropertyInteraction* pInteraction);
	void instantiatePropagator(const CvGameObject* pObject, CvPropertyPropagator* pPropagator);
	void instantiateManipulators(const CvGameObject* pObject, const CvPropertyManipulators* pMani);
	void instantiateGlobalManipulators(const CvGameObject* pObject);
	void gatherActiveManipulators();

	void predictSources();
	void correctSources();

	void predictInteractions();
	void correctInteractions();

	void predictPropagators();
	void correctPropagators();

	std::vector<int>& getCache1();
	std::vector<int>& getCache2();

	// Passed on to the solver map
	void addChange(const CvGameObject* pObject, PropertyTypes eProperty, int iChange);
	int getPredictValue(const CvGameObject* pObject, PropertyTypes eProperty);

protected:
	void gatherGlobalManipulators();
	void resetPropertyChanges();
	void gatherAndSolve();

	std::vector<PropertySourceContext> m_aSourceContexts;
	std::vector<PropertyInteractionContext> m_aInteractionContexts;
	std::vector<PropertyPropagatorContext> m_aPropagatorContexts;
	std::vector<int> m_aiCache1;
	std::vector<int> m_aiCache2;

	std::vector<const CvPropertyManipulators*> m_apGlobalManipulators;
	PropertySolverMap m_mapProperties;
};

#endif