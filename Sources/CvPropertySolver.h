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

typedef std::map<CvGameObject*, CvProperties> PropertySolverMapType;

class PropertySolverMap
{
public:
	int getPredictValue(CvGameObject* pObject, PropertyTypes eProperty);
	void addChange(CvGameObject* pObject, PropertyTypes eProperty, int iChange);

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
	PropertySourceContext(CvPropertySource* pSource, CvGameObject* pObject);
	CvPropertySource* getSource() const;
	CvGameObject* getObject() const;
	int getData1() const;
	int getData2() const;
	void setData1(int iData);
	void setData2(int iData);

	void doPredict(CvPropertySolver* pSolver);
	void doCorrect(CvPropertySolver* pSolver);

protected:
	CvPropertySource* m_pSource;
	CvGameObject* m_pObject;
	int m_iCurrentAmount;
	int m_iData1;
	int m_iData2;
};

class PropertyInteractionContext
{
public:
	PropertyInteractionContext(CvPropertyInteraction* pInteraction, CvGameObject* pObject);
	CvPropertyInteraction* getInteraction() const;
	CvGameObject* getObject() const;

	void doPredict(CvPropertySolver* pSolver);
	void doCorrect(CvPropertySolver* pSolver);

protected:
	CvPropertyInteraction* m_pInteraction;
	CvGameObject* m_pObject;
	int m_iCurrentAmountSource;
	int m_iCurrentAmountTarget;
};

class PropertyPropagatorContext
{
public:
	PropertyPropagatorContext(CvPropertyPropagator* pPropagator, CvGameObject* pObject);
	CvPropertyPropagator* getPropagator() const;
	CvGameObject* getObject() const;
	std::vector<CvGameObject*>* getTargetObjects();

	void doPredict(CvPropertySolver* pSolver);
	void doCorrect(CvPropertySolver* pSolver);

protected:
	CvPropertyPropagator* m_pPropagator;
	CvGameObject* m_pObject;
	std::vector<CvGameObject*> m_apTargetObjects;
	std::vector<int> m_aiCurrentAmount;
};

class CvMainPropertySolver;

class CvPropertySolver
{
public:
	void instantiateSource(CvGameObject* pObject, CvPropertySource* pSource);
	void instantiateInteraction(CvGameObject* pObject, CvPropertyInteraction* pInteraction);
	void instantiatePropagator(CvGameObject* pObject, CvPropertyPropagator* pPropagator);
	void instantiateManipulators(CvGameObject* pObject, const CvPropertyManipulators* pMani);
	void instantiateGlobalManipulators(CvGameObject* pObject);
	void gatherActiveManipulators();
	
	void predictSources();
	void correctSources();
	void clearSources();
	
	void predictInteractions();
	void correctInteractions();
	void clearInteractions();

	void predictPropagators();
	void correctPropagators();
	void clearPropagators();

	void setMainSolver(CvMainPropertySolver* pMainSolver);

	std::vector<int>& getCache1();
	std::vector<int>& getCache2();

	// Passed on to the solver map
	void addChange(CvGameObject* pObject, PropertyTypes eProperty, int iChange);
	int getPredictValue(CvGameObject* pObject, PropertyTypes eProperty) const;

protected:
	std::vector<PropertySourceContext> m_aSourceContexts;
	std::vector<PropertyInteractionContext> m_aInteractionContexts;
	std::vector<PropertyPropagatorContext> m_aPropagatorContexts;
	CvMainPropertySolver* m_pMainSolver;
	std::vector<int> m_aiCache1;
	std::vector<int> m_aiCache2;
};

class CvMainPropertySolver
{
public:
	CvMainPropertySolver();

	PropertySolverMap* getSolverMap();

	void addGlobalManipulators(const CvPropertyManipulators* pMani);
	void gatherGlobalManipulators();
	int getNumGlobalManipulators() const;
	const CvPropertyManipulators* getGlobalManipulator(int index) const;

	void resetPropertyChanges();

	void gatherAndSolve();

	void doTurn();

protected:
	std::vector<const CvPropertyManipulators*> m_apGlobalManipulators;
	CvPropertySolver m_Solver;
	PropertySolverMap m_mapProperties;
};


#endif