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
	const std::vector<CvPropertySource*>& getSources() const { return m_apSources; }
	const python::list cyGetSources() const;
	int addSource(PropertySourceTypes eType);

	int getNumInteractions() const;
	//CvPropertyInteraction* getInteraction(int index) const;
	const std::vector<CvPropertyInteraction*>& getInteractions() const { return m_apInteractions; }
	int addInteraction(PropertyInteractionTypes eType);

	int getNumPropagators() const;
	//CvPropertyPropagator* getPropagator(int index) const;
	const std::vector<CvPropertyPropagator*>& getPropagators() const { return m_apPropagators; }
	int addPropagator(PropertyPropagatorTypes eType);

	void buildDisplayString(CvWStringBuffer& szBuffer) const;

	bool read(CvXMLLoadUtility* pXML, const wchar_t* szTagName = L"PropertyManipulators");
	void copyNonDefaults(const CvPropertyManipulators* pProp);

	void getCheckSum(uint32_t& iSum) const;

protected:
	std::vector<CvPropertySource*> m_apSources;
	std::vector<CvPropertyInteraction*> m_apInteractions;
	std::vector<CvPropertyPropagator*> m_apPropagators;
};

#endif