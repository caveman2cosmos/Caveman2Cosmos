//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertySolver.cpp
//
//  PURPOSE: Singleton class for solving the system of property manipulators
//
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvGameAI.h"

PropertySourceContext::PropertySourceContext(CvPropertySource *pSource, CvGameObject *pObject) : m_pSource(pSource), m_pObject(pObject), m_iData1(0), m_iData2(0)
{
	m_iCurrentAmount = pObject->getPropertiesConst()->getValueByProperty(pSource->getProperty());
}

CvPropertySource* PropertySourceContext::getSource()
{
	return m_pSource;
}

CvGameObject* PropertySourceContext::getObject()
{
	return m_pObject;
}

int PropertySourceContext::getData1()
{
	return m_iData1;
}

int PropertySourceContext::getData2()
{
	return m_iData2;
}

void PropertySourceContext::setData1(int iData)
{
	m_iData1 = iData;
}

void PropertySourceContext::setData2(int iData)
{
	m_iData2 = iData;
}

void PropertySourceContext::doPredict(CvPropertySolver* pSolver)
{
	int iChange = m_pSource->getSourcePredict(m_pObject, m_iCurrentAmount, this);
	pSolver->addChange(m_pObject, m_pSource->getProperty(), iChange);
}

void PropertySourceContext::doCorrect(CvPropertySolver* pSolver)
{
	PropertyTypes eProperty = m_pSource->getProperty();
	int iPredicted = pSolver->getPredictValue(m_pObject, eProperty);
	int iChange = m_pSource->getSourceCorrect(m_pObject, m_iCurrentAmount, iPredicted, this);
	pSolver->addChange(m_pObject, eProperty, iChange);
}


PropertyInteractionContext::PropertyInteractionContext(CvPropertyInteraction *pInteraction, CvGameObject *pObject) : m_pInteraction(pInteraction), m_pObject(pObject)
{
	m_iCurrentAmountSource = pObject->getPropertiesConst()->getValueByProperty(pInteraction->getSourceProperty());
	m_iCurrentAmountTarget = pObject->getPropertiesConst()->getValueByProperty(pInteraction->getTargetProperty());
}

CvPropertyInteraction* PropertyInteractionContext::getInteraction()
{
	return m_pInteraction;
}

CvGameObject* PropertyInteractionContext::getObject()
{
	return m_pObject;
}

void PropertyInteractionContext::doPredict(CvPropertySolver* pSolver)
{
	std::pair<int,int> iChange = m_pInteraction->getPredict(m_iCurrentAmountSource, m_iCurrentAmountTarget);
	pSolver->addChange(m_pObject, m_pInteraction->getSourceProperty(), iChange.first);
	pSolver->addChange(m_pObject, m_pInteraction->getTargetProperty(), iChange.second);
}

void PropertyInteractionContext::doCorrect(CvPropertySolver* pSolver)
{
	PropertyTypes ePropertySource = m_pInteraction->getSourceProperty();
	PropertyTypes ePropertyTarget = m_pInteraction->getTargetProperty();
	int iPredictedAmountSource = pSolver->getPredictValue(m_pObject, ePropertySource);
	int iPredictedAmountTarget = pSolver->getPredictValue(m_pObject, ePropertyTarget);
	std::pair<int,int> iChange = m_pInteraction->getCorrect(m_iCurrentAmountSource, m_iCurrentAmountTarget, iPredictedAmountSource, iPredictedAmountTarget);
	pSolver->addChange(m_pObject, ePropertySource, iChange.first);
	pSolver->addChange(m_pObject, ePropertyTarget, iChange.second);
}


PropertyPropagatorContext::PropertyPropagatorContext(CvPropertyPropagator *pPropagator, CvGameObject *pObject) : m_pPropagator(pPropagator), m_pObject(pObject)
{
	pPropagator->getTargetObjects(pObject, m_apTargetObjects);
	PropertyTypes eProperty = pPropagator->getProperty();

	for(int i=0; i<(int)m_apTargetObjects.size(); i++)
	{
		m_aiCurrentAmount.push_back(m_apTargetObjects[i]->getPropertiesConst()->getValueByProperty(eProperty));
	}
}

CvPropertyPropagator* PropertyPropagatorContext::getPropagator()
{
	return m_pPropagator;
}

CvGameObject* PropertyPropagatorContext::getObject()
{
	return m_pObject;
}

std::vector<CvGameObject*>* PropertyPropagatorContext::getTargetObjects()
{
	return &m_apTargetObjects;
}

void PropertyPropagatorContext::doPredict(CvPropertySolver* pSolver)
{
	std::vector<int>& aiPredict = pSolver->getCache1();
	aiPredict.resize(m_aiCurrentAmount.size());
	m_pPropagator->getPredict(m_aiCurrentAmount, aiPredict);

	PropertyTypes eProperty = m_pPropagator->getProperty();
	for(int i=0; i<(int)aiPredict.size(); i++)
	{
		pSolver->addChange(m_apTargetObjects[i], eProperty, aiPredict[i]);
	}
}

void PropertyPropagatorContext::doCorrect(CvPropertySolver* pSolver)
{
	PropertyTypes eProperty = m_pPropagator->getProperty();
	std::vector<int>& aiPredict = pSolver->getCache1();
	for(int i=0; i<(int)m_apTargetObjects.size(); i++)
	{
		aiPredict.push_back(pSolver->getPredictValue(m_apTargetObjects[i], eProperty));
	}
	std::vector<int> aiCorrect = pSolver->getCache2();
	aiCorrect.resize(m_aiCurrentAmount.size());
	m_pPropagator->getCorrect(m_aiCurrentAmount, aiPredict, aiCorrect);
	for(int i=0; i<(int)aiCorrect.size(); i++)
	{
		pSolver->addChange(m_apTargetObjects[i], eProperty, aiCorrect[i]);
	}
}


void PropertySolverMap::addChange(CvGameObject *pObject, PropertyTypes eProperty, int iChange)
{
	m_mapPropertyChanges[pObject].changeValueByProperty(eProperty, iChange);
}

int PropertySolverMap::getPredictValue(CvGameObject *pObject, PropertyTypes eProperty)
{
	return m_mapProperties[pObject].getValueByProperty(eProperty);
}

void PropertySolverMap::computePredictValues()
{
	for (PropertySolverMapType::iterator it = m_mapPropertyChanges.begin(); it != m_mapPropertyChanges.end(); ++it)
	{
		CvGameObject* pObject = it->first;
		PropertySolverMapType::iterator it2 = m_mapProperties.find(pObject);
		if (it2 == m_mapProperties.end())
		{
			CvProperties* pObjProp = pObject->getProperties();
			it2 = m_mapProperties.insert(std::make_pair(pObject, CvProperties())).first;
			it2->second.addProperties(pObjProp);
		}
		it2->second.addProperties(&(it->second));
	}
	m_mapPropertyChanges.clear();
}


void PropertySolverMap::applyChanges()
{
	for(PropertySolverMapType::iterator it = m_mapPropertyChanges.begin(); it != m_mapPropertyChanges.end(); ++it)
	{
		CvProperties* pProp = it->first->getProperties();
		pProp->addProperties(&(it->second));
	}
	m_mapPropertyChanges.clear();

	// Changes are applied, clear the intermediate values
	m_mapProperties.clear();
}



void CvPropertySolver::instantiateSource(CvGameObject* pObject, CvPropertySource* pSource)
{
	if (pSource->isActive(pObject))
	{
		m_aSourceContexts.push_back(PropertySourceContext(pSource, pObject));
	}
}

void callInstantiateSource(CvGameObject* pObject, CvPropertySource* pSource, CvPropertySolver* pSolver)
{
	pSolver->instantiateSource(pObject, pSource);
}

void CvPropertySolver::instantiateInteraction(CvGameObject* pObject, CvPropertyInteraction* pInteraction)
{
	if (pInteraction->isActive(pObject))
	{
		m_aInteractionContexts.push_back(PropertyInteractionContext(pInteraction, pObject));
	}
}

void callInstantiateInteraction(CvGameObject* pObject, CvPropertyInteraction* pInteraction, CvPropertySolver* pSolver)
{
	pSolver->instantiateInteraction(pObject, pInteraction);
}

void CvPropertySolver::instantiatePropagator(CvGameObject* pObject, CvPropertyPropagator* pPropagator)
{
	if (pPropagator->isActive(pObject))
	{
		m_aPropagatorContexts.push_back(PropertyPropagatorContext(pPropagator, pObject));
	}
}

void callInstantiatePropagator(CvGameObject* pObject, CvPropertyPropagator* pPropagator, CvPropertySolver* pSolver)
{
	pSolver->instantiatePropagator(pObject, pPropagator);
}

void CvPropertySolver::instantiateManipulators(CvGameObject* pObject, CvPropertyManipulators* pMani)
{
	// Sources
	for (int j=0; j<pMani->getNumSources(); j++)
	{
		CvPropertySource* pSource = pMani->getSource(j);
		RelationTypes eRelation = pSource->getRelation();
		if (eRelation == NO_RELATION)
		{
			instantiateSource(pObject, pSource);
		}
		else
		{
			pObject->foreachRelated(pSource->getObjectType(), eRelation, bst::bind(callInstantiateSource, _1, pSource, this), pSource->getRelationData());
		}
	}
	// Interactions
	for (int j=0; j<pMani->getNumInteractions(); j++)
	{
		CvPropertyInteraction* pInteraction = pMani->getInteraction(j);
		RelationTypes eRelation = pInteraction->getRelation();
		if (eRelation == NO_RELATION)
		{
			instantiateInteraction(pObject, pInteraction);
		}
		else
		{
			pObject->foreachRelated(pInteraction->getObjectType(), eRelation, bst::bind(callInstantiateInteraction, _1, pInteraction, this), pInteraction->getRelationData());
		}
	}
	// Propagators
	for (int j=0; j<pMani->getNumPropagators(); j++)
	{
		CvPropertyPropagator* pPropagator = pMani->getPropagator(j);
		RelationTypes eRelation = pPropagator->getRelation();
		if (eRelation == NO_RELATION)
		{
			instantiatePropagator(pObject, pPropagator);
		}
		else
		{
			pObject->foreachRelated(pPropagator->getObjectType(), eRelation, bst::bind(callInstantiatePropagator, _1, pPropagator, this), pPropagator->getRelationData());
		}
	}
}

void CvPropertySolver::instantiateGlobalManipulators(CvGameObject *pObject)
{
	for (int i=0; i < m_pMainSolver->getNumGlobalManipulators(); i++)
	{
		instantiateManipulators(pObject, m_pMainSolver->getGlobalManipulator(i));
	}
}

// helper functions
void callInstantiateManipulators(CvGameObject* pObject, CvPropertyManipulators* pMani, CvPropertySolver* pSolver)
{
	pSolver->instantiateManipulators(pObject, pMani);
}

void callInstantiateGlobalManipulators(CvGameObject* pObject, CvPropertySolver* pSolver)
{
	pSolver->instantiateGlobalManipulators(pObject);
	pObject->foreachManipulator(bst::bind(callInstantiateManipulators, pObject, _1, pSolver));
}

void CvPropertySolver::gatherActiveManipulators()
{
	for (int i=0; i<NUM_GAMEOBJECTS; i++)
	{
		GC.getGame().getGameObject()->foreach((GameObjectTypes)i, bst::bind(callInstantiateGlobalManipulators, _1, this));
	}
}

void CvPropertySolver::predictSources()
{
	for (int i=0; i<(int)m_aSourceContexts.size(); i++)
	{
		m_aSourceContexts[i].doPredict(this);
	}
}

void CvPropertySolver::correctSources()
{
	for (int i=0; i<(int)m_aSourceContexts.size(); i++)
	{
		m_aSourceContexts[i].doCorrect(this);
	}
}

void CvPropertySolver::clearSources()
{
	m_aSourceContexts.clear();
}

void CvPropertySolver::predictInteractions()
{
	for (int i=0; i<(int)m_aInteractionContexts.size(); i++)
	{
		m_aInteractionContexts[i].doPredict(this);
	}
}

void CvPropertySolver::correctInteractions()
{
	for (int i=0; i<(int)m_aInteractionContexts.size(); i++)
	{
		m_aInteractionContexts[i].doCorrect(this);
	}
}

void CvPropertySolver::clearInteractions()
{
	m_aInteractionContexts.clear();
}

void CvPropertySolver::predictPropagators()
{
	for (int i=0; i<(int)m_aPropagatorContexts.size(); i++)
	{
		m_aPropagatorContexts[i].doPredict(this);
	}
}

void CvPropertySolver::correctPropagators()
{
	for (int i=0; i<(int)m_aPropagatorContexts.size(); i++)
	{
		m_aPropagatorContexts[i].doCorrect(this);
	}
}

void CvPropertySolver::clearPropagators()
{
	m_aPropagatorContexts.clear();
}

void CvPropertySolver::setMainSolver(CvMainPropertySolver *pMainSolver)
{
	m_pMainSolver = pMainSolver;
}


void callResetPropertyChange(CvGameObject* pObject)
{
	pObject->getProperties()->clearChange();
}

std::vector<int>& CvPropertySolver::getCache1()
{
	return m_aiCache1;
}

std::vector<int>& CvPropertySolver::getCache2()
{
	return m_aiCache2;
}

int CvPropertySolver::getPredictValue(CvGameObject *pObject, PropertyTypes eProperty)
{
	return m_pMainSolver->getSolverMap()->getPredictValue(pObject, eProperty);
}

void CvPropertySolver::addChange(CvGameObject* pObject, PropertyTypes eProperty, int iChange)
{
	m_pMainSolver->getSolverMap()->addChange(pObject, eProperty, iChange);
}

void CvMainPropertySolver::resetPropertyChanges()
{
	for (int i=0; i<NUM_GAMEOBJECTS; i++)
	{
		GC.getGame().getGameObject()->foreach((GameObjectTypes)i, callResetPropertyChange);
	}
}

CvMainPropertySolver::CvMainPropertySolver()
{
	m_Solver.setMainSolver(this);
}

PropertySolverMap* CvMainPropertySolver::getSolverMap()
{
	return &m_mapProperties;
}

void CvMainPropertySolver::addGlobalManipulators(CvPropertyManipulators *pMani)
{
	m_apGlobalManipulators.push_back(pMani);
}

void CvMainPropertySolver::gatherGlobalManipulators()
{
	// Global manipulators first
	for (int i=0; i<GC.getNumPropertyInfos(); i++)
	{
		addGlobalManipulators(GC.getPropertyInfo((PropertyTypes)i).getPropertyManipulators());
	}
}

int CvMainPropertySolver::getNumGlobalManipulators()
{
	return (int)m_apGlobalManipulators.size();
}

CvPropertyManipulators* CvMainPropertySolver::getGlobalManipulator(int index)
{
	return m_apGlobalManipulators[index];
}

void CvMainPropertySolver::gatherAndSolve()
{
	m_Solver.gatherActiveManipulators();

	// Propagators first
	m_Solver.predictPropagators();
	m_mapProperties.computePredictValues();
	m_Solver.correctPropagators();
	m_mapProperties.applyChanges();
	m_Solver.clearPropagators();

	// Interactions next
	m_Solver.predictInteractions();
	m_mapProperties.computePredictValues();
	m_Solver.correctInteractions();
	m_mapProperties.applyChanges();
	m_Solver.clearInteractions();

	// Sources last
	m_Solver.predictSources();
	m_mapProperties.computePredictValues();
	m_Solver.correctSources();
	m_mapProperties.applyChanges();
	m_Solver.clearSources();
}


void CvMainPropertySolver::doTurn()
{
	MEMORY_TRACE_FUNCTION();
	PROFILE_FUNC();

	resetPropertyChanges();
	
	gatherGlobalManipulators();

	gatherAndSolve();
	
	m_apGlobalManipulators.clear();
}
	
