//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertyManipulators.cpp
//
//  PURPOSE: Stores manipulators of generic properties for Civ4 classes (sources, interactions, propagators)
//
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvPropertyManipulators::~CvPropertyManipulators()
{
	for (int i=0; i<(int)m_apSources.size(); i++)
	{
		delete m_apSources[i];
	}
	for (int i=0; i<(int)m_apInteractions.size(); i++)
	{
		delete m_apInteractions[i];
	}
	for (int i=0; i<(int)m_apPropagators.size(); i++)
	{
		delete m_apPropagators[i];
	}
}

int CvPropertyManipulators::getNumSources() const
{
	return (int) m_apSources.size();
}
/*
CvPropertySource* CvPropertyManipulators::getSource(int index) const
{
	FASSERT_BOUNDS(0, getNumSources(), index)
	return m_apSources[index];
}
*/
int CvPropertyManipulators::addSource(PropertySourceTypes eType)
{
	switch (eType)
	{
		case PROPERTYSOURCE_CONSTANT:
			m_apSources.push_back(new CvPropertySourceConstant());
			return (int)m_apSources.size()-1;
			break;
		case PROPERTYSOURCE_CONSTANT_LIMITED:
			m_apSources.push_back(new CvPropertySourceConstantLimited());
			return (int)m_apSources.size()-1;
			break;
		case PROPERTYSOURCE_DECAY:
			m_apSources.push_back(new CvPropertySourceDecay());
			return (int)m_apSources.size()-1;
			break;
		case PROPERTYSOURCE_ATTRIBUTE_CONSTANT:
			m_apSources.push_back(new CvPropertySourceAttributeConstant());
			return (int)m_apSources.size()-1;
			break;
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
	FASSERT_BOUNDS(0, getNumInteractions(), index)
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
			break;

		case PROPERTYINTERACTION_INHIBITED_GROWTH:
			m_apInteractions.push_back(new CvPropertyInteractionInhibitedGrowth());
			return (int)m_apInteractions.size()-1;
			break;

		case PROPERTYINTERACTION_CONVERT_PERCENT:
			m_apInteractions.push_back(new CvPropertyInteractionConvertPercent());
			return (int)m_apInteractions.size()-1;
			break;
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
	FASSERT_BOUNDS(0, getNumPropagators(), index)
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
			break;

		case PROPERTYPROPAGATOR_GATHER:
			m_apPropagators.push_back(new CvPropertyPropagatorGather());
			return (int)m_apPropagators.size()-1;
			break;

		case PROPERTYPROPAGATOR_DIFFUSE:
			m_apPropagators.push_back(new CvPropertyPropagatorDiffuse());
			return (int)m_apPropagators.size()-1;
			break;
	}
	return -1;
}

void CvPropertyManipulators::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	for (int i=0; i<(int)m_apSources.size(); i++)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		m_apSources[i]->buildDisplayString(szBuffer);
	}
	for (int i=0; i<(int)m_apInteractions.size(); i++)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		m_apInteractions[i]->buildDisplayString(szBuffer);
	}
	for (int i=0; i<(int)m_apPropagators.size(); i++)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		m_apPropagators[i]->buildDisplayString(szBuffer);
	}
}

bool CvPropertyManipulators::read(CvXMLLoadUtility *pXML, const wchar_t* szTagName)
{
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
					int iType = pXML->GetInfoClass(szTextVal);
					int iPos = addSource((PropertySourceTypes)iType);
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
					int iType = pXML->GetInfoClass(szTextVal);
					int iPos = addInteraction((PropertyInteractionTypes)iType);
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
					int iType = pXML->GetInfoClass(szTextVal);
					int iPos = addPropagator((PropertyPropagatorTypes)iType);
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

void CvPropertyManipulators::copyNonDefaults(CvPropertyManipulators *pProp, CvXMLLoadUtility *pXML)
{
	//if (m_apSources.empty())
	{
		foreach_(CvPropertySource* pSource, pProp->getSources())
		{
			const int iPos = addSource(pSource->getType());
			if (iPos != -1)
				m_apSources[iPos]->copyNonDefaults(pSource, pXML);
		}
	}
	//if (m_apInteractions.empty())
	{
		foreach_(CvPropertyInteraction* pInteraction, pProp->getInteractions())
		{
			const int iPos = addInteraction(pInteraction->getType());
			if (iPos != -1)
				m_apInteractions[iPos]->copyNonDefaults(pInteraction, pXML);
		}
	}
	//if (m_apPropagators.empty())
	{
		foreach_(CvPropertyPropagator* pPropagator, pProp->getPropagators())
		{
			const int iPos = addPropagator(pPropagator->getType());
			if (iPos != -1)
				m_apPropagators[iPos]->copyNonDefaults(pPropagator, pXML);
		}
	}
}

void CvPropertyManipulators::getCheckSum(unsigned int &iSum) const
{
	for(int i=0;i<(int)m_apSources.size();i++)
	{
		m_apSources[i]->getCheckSum(iSum);
	}
	for(int i=0;i<(int)m_apInteractions.size();i++)
	{
		m_apInteractions[i]->getCheckSum(iSum);
	}
	for(int i=0;i<(int)m_apPropagators.size();i++)
	{
		m_apPropagators[i]->getCheckSum(iSum);
	}
}
