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
#include "CvGlobals.h"

PropertySourceContext::PropertySourceContext(CvPropertySource* pSource, const CvGameObject* pObject) : m_pSource(pSource), m_pObject(pObject), m_iData1(0), m_iData2(0)
{
	m_iCurrentAmount = pObject->getPropertiesConst()->getValueByProperty(pSource->getProperty());
}

CvPropertySource* PropertySourceContext::getSource() const
{
	return m_pSource;
}

const CvGameObject* PropertySourceContext::getObject() const
{
	return m_pObject;
}

int PropertySourceContext::getData1() const
{
	return m_iData1;
}

int PropertySourceContext::getData2() const
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
	const int iChange = m_pSource->getSourcePredict(m_pObject, m_iCurrentAmount, this);
	pSolver->addChange(m_pObject, m_pSource->getProperty(), iChange);
}

void PropertySourceContext::doCorrect(CvPropertySolver* pSolver)
{
	const PropertyTypes eProperty = m_pSource->getProperty();
	const int iPredicted = pSolver->getPredictValue(m_pObject, eProperty);
	const int iChange = m_pSource->getSourceCorrect(m_pObject, m_iCurrentAmount, iPredicted, this);
	pSolver->addChange(m_pObject, eProperty, iChange);
}


PropertyInteractionContext::PropertyInteractionContext(CvPropertyInteraction* pInteraction, const CvGameObject* pObject) : m_pInteraction(pInteraction), m_pObject(pObject)
{
	m_iCurrentAmountSource = pObject->getPropertiesConst()->getValueByProperty(pInteraction->getSourceProperty());
	m_iCurrentAmountTarget = pObject->getPropertiesConst()->getValueByProperty(pInteraction->getTargetProperty());
}

CvPropertyInteraction* PropertyInteractionContext::getInteraction() const
{
	return m_pInteraction;
}

const CvGameObject* PropertyInteractionContext::getObject() const
{
	return m_pObject;
}

void PropertyInteractionContext::doPredict(CvPropertySolver* pSolver)
{
	const std::pair<int,int> iChange = m_pInteraction->getPredict(m_iCurrentAmountSource, m_iCurrentAmountTarget);
	pSolver->addChange(m_pObject, m_pInteraction->getSourceProperty(), iChange.first);
	pSolver->addChange(m_pObject, m_pInteraction->getTargetProperty(), iChange.second);
}

void PropertyInteractionContext::doCorrect(CvPropertySolver* pSolver)
{
	const PropertyTypes ePropertySource = m_pInteraction->getSourceProperty();
	const PropertyTypes ePropertyTarget = m_pInteraction->getTargetProperty();
	const int iPredictedAmountSource = pSolver->getPredictValue(m_pObject, ePropertySource);
	const int iPredictedAmountTarget = pSolver->getPredictValue(m_pObject, ePropertyTarget);
	const std::pair<int,int> iChange = m_pInteraction->getCorrect(m_iCurrentAmountSource, m_iCurrentAmountTarget, iPredictedAmountSource, iPredictedAmountTarget);
	pSolver->addChange(m_pObject, ePropertySource, iChange.first);
	pSolver->addChange(m_pObject, ePropertyTarget, iChange.second);
}


PropertyPropagatorContext::PropertyPropagatorContext(CvPropertyPropagator* pPropagator, const CvGameObject* pObject) : m_pPropagator(pPropagator), m_pObject(pObject)
{
	pPropagator->getTargetObjects(pObject, m_apTargetObjects);
	const PropertyTypes eProperty = pPropagator->getProperty();

	foreach_(const CvGameObject* object, m_apTargetObjects)
	{
		m_aiCurrentAmount.push_back(object->getPropertiesConst()->getValueByProperty(eProperty));
	}
}

CvPropertyPropagator* PropertyPropagatorContext::getPropagator() const
{
	return m_pPropagator;
}

const CvGameObject* PropertyPropagatorContext::getObject() const
{
	return m_pObject;
}

std::vector<const CvGameObject*>* PropertyPropagatorContext::getTargetObjects()
{
	return &m_apTargetObjects;
}

void PropertyPropagatorContext::doPredict(CvPropertySolver* pSolver)
{
	std::vector<int>& aiPredict = pSolver->getCache1();
	aiPredict.resize(m_aiCurrentAmount.size());
	m_pPropagator->getPredict(m_aiCurrentAmount, aiPredict);

	const PropertyTypes eProperty = m_pPropagator->getProperty();
	for(int i=0; i<(int)aiPredict.size(); i++)
	{
		pSolver->addChange(m_apTargetObjects[i], eProperty, aiPredict[i]);
	}
}

void PropertyPropagatorContext::doCorrect(CvPropertySolver* pSolver)
{
	const PropertyTypes eProperty = m_pPropagator->getProperty();
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


void PropertySolverMap::addChange(const CvGameObject* pObject, PropertyTypes eProperty, int iChange)
{
	m_mapPropertyChanges[pObject].changeValueByProperty(eProperty, iChange);
}

int PropertySolverMap::getPredictValue(const CvGameObject* pObject, PropertyTypes eProperty)
{
	return m_mapProperties[pObject].getValueByProperty(eProperty);
}

void PropertySolverMap::computePredictValues()
{
	for (PropertySolverMapType::iterator it = m_mapPropertyChanges.begin(); it != m_mapPropertyChanges.end(); ++it)
	{
		const CvGameObject* pObject = it->first;
		PropertySolverMapType::iterator it2 = m_mapProperties.find(pObject);
		if (it2 == m_mapProperties.end())
		{
			const CvProperties* pObjProp = pObject->getProperties();
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



void CvPropertySolver::instantiateSource(const CvGameObject* pObject, CvPropertySource* pSource)
{
	if (pSource->isActive(pObject))
	{
		m_aSourceContexts.push_back(PropertySourceContext(pSource, pObject));
	}
}

void callInstantiateSource(const CvGameObject* pObject, CvPropertySource* pSource, CvPropertySolver* pSolver)
{
	pSolver->instantiateSource(pObject, pSource);
}

void CvPropertySolver::instantiateInteraction(const CvGameObject* pObject, CvPropertyInteraction* pInteraction)
{
	if (pInteraction->isActive(pObject))
	{
		m_aInteractionContexts.push_back(PropertyInteractionContext(pInteraction, pObject));
	}
}

void callInstantiateInteraction(const CvGameObject* pObject, CvPropertyInteraction* pInteraction, CvPropertySolver* pSolver)
{
	pSolver->instantiateInteraction(pObject, pInteraction);
}

void CvPropertySolver::instantiatePropagator(const CvGameObject* pObject, CvPropertyPropagator* pPropagator)
{
	if (pPropagator->isActive(pObject))
	{
		m_aPropagatorContexts.push_back(PropertyPropagatorContext(pPropagator, pObject));
	}
}

void callInstantiatePropagator(const CvGameObject* pObject, CvPropertyPropagator* pPropagator, CvPropertySolver* pSolver)
{
	pSolver->instantiatePropagator(pObject, pPropagator);
}

void CvPropertySolver::instantiateManipulators(const CvGameObject* pObject, const CvPropertyManipulators* pMani)
{
	// Sources
	foreach_(CvPropertySource* pSource, pMani->getSources())
	{
		const RelationTypes eRelation = pSource->getRelation();
		if (eRelation == NO_RELATION)
		{
			instantiateSource(pObject, pSource);
		}
		else
		{
			pObject->foreachRelated(pSource->getObjectType(), eRelation, bind(callInstantiateSource, _1, pSource, this), pSource->getRelationData());
		}
	}
	// Interactions
	foreach_(CvPropertyInteraction* pInteraction, pMani->getInteractions())
	{
		const RelationTypes eRelation = pInteraction->getRelation();
		if (eRelation == NO_RELATION)
		{
			instantiateInteraction(pObject, pInteraction);
		}
		else
		{
			pObject->foreachRelated(pInteraction->getObjectType(), eRelation, bind(callInstantiateInteraction, _1, pInteraction, this), pInteraction->getRelationData());
		}
	}
	// Propagators
	foreach_(CvPropertyPropagator* pPropagator, pMani->getPropagators())
	{
		const RelationTypes eRelation = pPropagator->getRelation();
		if (eRelation == NO_RELATION)
		{
			instantiatePropagator(pObject, pPropagator);
		}
		else
		{
			pObject->foreachRelated(pPropagator->getObjectType(), eRelation, bind(callInstantiatePropagator, _1, pPropagator, this), pPropagator->getRelationData());
		}
	}
}

void CvPropertySolver::instantiateGlobalManipulators(const CvGameObject *pObject)
{
	foreach_(const CvPropertyManipulators* pMani, m_apGlobalManipulators)
	{
		instantiateManipulators(pObject, pMani);
	}
}

// helper functions
void callInstantiateManipulators(const CvGameObject* pObject, const CvPropertyManipulators* pMani, CvPropertySolver* pSolver)
{
	pSolver->instantiateManipulators(pObject, pMani);
}

void callInstantiateGlobalManipulators(const CvGameObject* pObject, CvPropertySolver* pSolver)
{
	pSolver->instantiateGlobalManipulators(pObject);
	pObject->foreachManipulator(bind(callInstantiateManipulators, pObject, _1, pSolver));
}

void CvPropertySolver::gatherActiveManipulators()
{
	for (int i=0; i<NUM_GAMEOBJECTS; i++)
	{
		GC.getGame().getGameObject()->foreach((GameObjectTypes)i, bind(callInstantiateGlobalManipulators, _1, this));
	}
}

void CvPropertySolver::predictSources()
{
	foreach_(PropertySourceContext& context, m_aSourceContexts)
	{
		context.doPredict(this);
	}
}

void CvPropertySolver::correctSources()
{
	foreach_(PropertySourceContext& context, m_aSourceContexts)
	{
		context.doCorrect(this);
	}
}

void CvPropertySolver::predictInteractions()
{
	foreach_(PropertyInteractionContext& context, m_aInteractionContexts)
	{
		context.doPredict(this);
	}
}

void CvPropertySolver::correctInteractions()
{
	foreach_(PropertyInteractionContext& context, m_aInteractionContexts)
	{
		context.doCorrect(this);
	}
}

void CvPropertySolver::predictPropagators()
{
	foreach_(PropertyPropagatorContext& context, m_aPropagatorContexts)
	{
		context.doPredict(this);
	}
}

void CvPropertySolver::correctPropagators()
{
	foreach_(PropertyPropagatorContext& context, m_aPropagatorContexts)
	{
		context.doCorrect(this);
	}
}


void callResetPropertyChange(const CvGameObject* pObject)
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

int CvPropertySolver::getPredictValue(const CvGameObject *pObject, PropertyTypes eProperty)
{
	return m_mapProperties.getPredictValue(pObject, eProperty);
}

void CvPropertySolver::addChange(const CvGameObject* pObject, PropertyTypes eProperty, int iChange)
{
	m_mapProperties.addChange(pObject, eProperty, iChange);
}

void CvPropertySolver::resetPropertyChanges()
{
	for (int i=0; i<NUM_GAMEOBJECTS; i++)
	{
		GC.getGame().getGameObject()->foreach((GameObjectTypes)i, callResetPropertyChange);
	}
}

void CvPropertySolver::gatherGlobalManipulators()
{
	// Global manipulators first
	for (int i=0; i<GC.getNumPropertyInfos(); i++)
	{
		m_apGlobalManipulators.push_back(GC.getPropertyInfo((PropertyTypes)i).getPropertyManipulators());
	}
}

void CvPropertySolver::gatherAndSolve()
{
	gatherActiveManipulators();

	// Propagators first
	predictPropagators();
	m_mapProperties.computePredictValues();
	correctPropagators();
	m_mapProperties.applyChanges();
	m_aPropagatorContexts.clear();

	// Interactions next
	predictInteractions();
	m_mapProperties.computePredictValues();
	correctInteractions();
	m_mapProperties.applyChanges();
	m_aInteractionContexts.clear();

	// Sources last
	predictSources();
	m_mapProperties.computePredictValues();
	correctSources();
	m_mapProperties.applyChanges();
	m_aSourceContexts.clear();
}


void CvPropertySolver::doTurn()
{
	PROFILE_FUNC();

	resetPropertyChanges();

	gatherGlobalManipulators();

	gatherAndSolve();

	m_apGlobalManipulators.clear();
}

