//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertyManipulators.cpp
//
//  PURPOSE: Stores manipulators of generic properties for Civ4 classes (sources, interactions, propagators)
//
//------------------------------------------------------------------------------------------------


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvPropertyInteraction.h"
#include "CvPropertyManipulators.h"
#include "CvPropertyPropagator.h"
#include "CvPropertySource.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvPropertyManipulators::~CvPropertyManipulators()
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPropertySource* pSource, m_apSources)
	{
		delete pSource;
	}
	foreach_(const CvPropertyInteraction* pInteraction, m_apInteractions)
	{
		delete pInteraction;
	}
	foreach_(const CvPropertyPropagator* pPropagator, m_apPropagators)
	{
		delete pPropagator;
	}
}

int CvPropertyManipulators::getNumSources() const
{
	return (int) m_apSources.size();
}

CvPropertySource* CvPropertyManipulators::getSource(int index) const
{
	FASSERT_BOUNDS(0, getNumSources(), index);
	return m_apSources[index];
}

const python::list CvPropertyManipulators::cyGetSources() const
{
	PROFILE_EXTRA_FUNC();
	python::list l = python::list();
	foreach_(const CvPropertySource* pSource, m_apSources)
		l += *pSource;
	return l;
}

int CvPropertyManipulators::addSource(PropertySourceTypes eType)
{
	switch (eType)
	{
		case PROPERTYSOURCE_CONSTANT:
			m_apSources.push_back(new CvPropertySourceConstant());
			return (int)m_apSources.size()-1;

		case PROPERTYSOURCE_CONSTANT_LIMITED:
			m_apSources.push_back(new CvPropertySourceConstantLimited());
			return (int)m_apSources.size()-1;

		case PROPERTYSOURCE_DECAY:
			m_apSources.push_back(new CvPropertySourceDecay());
			return (int)m_apSources.size()-1;

		case PROPERTYSOURCE_ATTRIBUTE_CONSTANT:
			m_apSources.push_back(new CvPropertySourceAttributeConstant());
			return (int)m_apSources.size()-1;
	}
	return -1;
}

int CvPropertyManipulators::getNumInteractions() const
{
	return (int) m_apInteractions.size();
}
/*
CvPropertyInteraction* CvPropertyManipulators::getInteraction(int index) const
{
	FASSERT_BOUNDS(0, getNumInteractions(), index);
	return m_apInteractions[index];
}
*/
int CvPropertyManipulators::addInteraction(PropertyInteractionTypes eType)
{
	switch (eType)
	{
		case PROPERTYINTERACTION_CONVERT_CONSTANT:
			m_apInteractions.push_back(new CvPropertyInteractionConvertConstant());
			return (int)m_apInteractions.size()-1;

		case PROPERTYINTERACTION_INHIBITED_GROWTH:
			m_apInteractions.push_back(new CvPropertyInteractionInhibitedGrowth());
			return (int)m_apInteractions.size()-1;

		case PROPERTYINTERACTION_CONVERT_PERCENT:
			m_apInteractions.push_back(new CvPropertyInteractionConvertPercent());
			return (int)m_apInteractions.size()-1;
	}
	return -1;
}

int CvPropertyManipulators::getNumPropagators() const
{
	return (int) m_apPropagators.size();
}
/*
CvPropertyPropagator* CvPropertyManipulators::getPropagator(int index) const
{
	FASSERT_BOUNDS(0, getNumPropagators(), index);
	return m_apPropagators[index];
}
*/
int CvPropertyManipulators::addPropagator(PropertyPropagatorTypes eType)
{
	switch (eType)
	{
		case PROPERTYPROPAGATOR_SPREAD:
			m_apPropagators.push_back(new CvPropertyPropagatorSpread());
			return (int)m_apPropagators.size()-1;

		case PROPERTYPROPAGATOR_GATHER:
			m_apPropagators.push_back(new CvPropertyPropagatorGather());
			return (int)m_apPropagators.size()-1;

		case PROPERTYPROPAGATOR_DIFFUSE:
			m_apPropagators.push_back(new CvPropertyPropagatorDiffuse());
			return (int)m_apPropagators.size()-1;
	}
	return -1;
}

void CvPropertyManipulators::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPropertySource* pSource, getSources())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		pSource->buildDisplayString(szBuffer);
	}
	foreach_(const CvPropertyInteraction* pInteraction, getInteractions())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		pInteraction->buildDisplayString(szBuffer);
	}
	foreach_(const CvPropertyPropagator* pPropagator, getPropagators())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		pPropagator->buildDisplayString(szBuffer);
	}
}

bool CvPropertyManipulators::read(CvXMLLoadUtility *pXML, const wchar_t* szTagName)
{
	PROFILE_EXTRA_FUNC();
	if(pXML->TryMoveToXmlFirstChild(szTagName))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertySource"))
			{
				do
				{
					CvString szTextVal;
					pXML->GetChildXmlValByName(szTextVal, L"PropertySourceType");
					const int iType = pXML->GetInfoClass(szTextVal);
					const int iPos = addSource((PropertySourceTypes)iType);
					if (iPos != -1)
					{
						m_apSources[iPos]->read(pXML);
					}
				} while(pXML->TryMoveToXmlNextSibling(L"PropertySource"));
			}
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertyInteraction"))
			{
				do
				{
					CvString szTextVal;
					pXML->GetChildXmlValByName(szTextVal, L"PropertyInteractionType");
					const int iType = pXML->GetInfoClass(szTextVal);
					const int iPos = addInteraction((PropertyInteractionTypes)iType);
					if (iPos != -1)
						m_apInteractions[iPos]->read(pXML);
				} while(pXML->TryMoveToXmlNextSibling(L"PropertyInteraction"));
			}
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertyPropagator"))
			{
				do
				{
					CvString szTextVal;
					pXML->GetChildXmlValByName(szTextVal, L"PropertyPropagatorType");
					const int iType = pXML->GetInfoClass(szTextVal);
					const int iPos = addPropagator((PropertyPropagatorTypes)iType);
					if (iPos != -1)
						m_apPropagators[iPos]->read(pXML);
				} while(pXML->TryMoveToXmlNextSibling(L"PropertyPropagator"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvPropertyManipulators::copyNonDefaults(const CvPropertyManipulators* pProp)
{
	PROFILE_EXTRA_FUNC();
	//if (m_apSources.empty())
	{
		foreach_(CvPropertySource* pSource, pProp->getSources())
		{
			const int iPos = addSource(pSource->getType());
			if (iPos != -1)
				m_apSources[iPos]->copyNonDefaults(pSource);
		}
	}
	//if (m_apInteractions.empty())
	{
		foreach_(CvPropertyInteraction* pInteraction, pProp->getInteractions())
		{
			const int iPos = addInteraction(pInteraction->getType());
			if (iPos != -1)
				m_apInteractions[iPos]->copyNonDefaults(pInteraction);
		}
	}
	//if (m_apPropagators.empty())
	{
		foreach_(CvPropertyPropagator* pPropagator, pProp->getPropagators())
		{
			const int iPos = addPropagator(pPropagator->getType());
			if (iPos != -1)
				m_apPropagators[iPos]->copyNonDefaults(pPropagator);
		}
	}
}

void CvPropertyManipulators::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPropertySource* pSource, getSources())
	{
		pSource->getCheckSum(iSum);
	}
	foreach_(const CvPropertyInteraction* pInteraction, getInteractions())
	{
		pInteraction->getCheckSum(iSum);
	}
	foreach_(const CvPropertyPropagator* pPropagator, getPropagators())
	{
		pPropagator->getCheckSum(iSum);
	}
}
