#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertyManipulators.h
//
//  PURPOSE: Stores manipulators of generic properties for Civ4 classes (sources, interactions, propagators)
//
//------------------------------------------------------------------------------------------------
#ifndef CV_PROPERTY_MANIPULATORS_H
#define CV_PROPERTY_MANIPULATORS_H

class CvPropertySource;
class CvPropertyInteraction;
class CvPropertyPropagator;

class CvPropertyManipulators
{
public:
	~CvPropertyManipulators();

	int getNumSources() const;
	CvPropertySource* getSource(int index) const;
	int addSource(PropertySourceTypes eType);

	int getNumInteractions() const;
	CvPropertyInteraction* getInteraction(int index) const;
	int addInteraction(PropertyInteractionTypes eType);

	int getNumPropagators() const;
	CvPropertyPropagator* getPropagator(int index) const;
	int addPropagator(PropertyPropagatorTypes eType);

	void buildDisplayString(CvWStringBuffer& szBuffer) const;

	bool read(CvXMLLoadUtility* pXML, const wchar_t* szTagName = L"PropertyManipulators");
	void copyNonDefaults(CvPropertyManipulators* pProp, CvXMLLoadUtility* pXML );

	void getCheckSum(unsigned int& iSum);

protected:
	std::vector<CvPropertySource*> m_apSources;
	std::vector<CvPropertyInteraction*> m_apInteractions;
	std::vector<CvPropertyPropagator*> m_apPropagators;
};

#endif