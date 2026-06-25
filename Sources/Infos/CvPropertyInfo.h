#pragma once

#ifndef CV_PROPERTY_INFO_H
#define CV_PROPERTY_INFO_H

#include "CvInfoBase.h"

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 END												  */
/************************************************************************************************/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPropertyInfo
//
//  DESC:   Contains info about generic properties which can be added to buildings
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPropertyInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvPropertyInfo();
	virtual ~CvPropertyInfo();

	int getAIWeight() const;
	AIScaleTypes getAIScaleType() const;
	int getOperationalRangeMin() const;
	int getOperationalRangeMax() const;
	int getTargetLevel() const;
	int getTrainReluctance() const;
	bool isSourceDrain() const;
	bool isOAType() const;
	CvWString getValueDisplayText() const;
	CvWString getChangeDisplayText() const;
	CvWString getChangeAllCitiesDisplayText() const;
	CvWString getPrereqMinDisplayText() const;
	CvWString getPrereqMaxDisplayText() const;

	const PropertyBuilding& getPropertyBuilding(int index) const;
	int getNumPropertyBuildings() const;
	const std::vector<PropertyBuilding>& getPropertyBuildings() const { return m_aPropertyBuildings; }

	const PropertyPromotion& getPropertyPromotion(int index) const;
	int getNumPropertyPromotions() const;
	const std::vector<PropertyPromotion>& getPropertyPromotions() const { return m_aPropertyPromotions; }

	int getChangePropagator(const GameObjectTypes eFrom, const GameObjectTypes eTo) const;

	int getNumTargetLevelbyEraTypes() const;
	int getTargetLevelbyEraType(int iIndex) const;
	bool isTargetLevelbyEraType(int iIndex) const;

	int getChar() const;
	void setChar(int i);
	int getFontButtonIndex() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvPropertyInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

private:
	CvPropertyManipulators m_PropertyManipulators;

protected:
	int m_iChar;
	int m_iFontButtonIndex;

	int m_iAIWeight;
	AIScaleTypes m_eAIScaleType;
	int m_iOperationalRangeMin;
	int m_iOperationalRangeMax;
	int m_iTargetLevel;
	int m_iTrainReluctance;
	bool m_bSourceDrain;
	bool m_bOAType;
	CvWString m_szValueDisplayText;
	CvWString m_szChangeDisplayText;
	CvWString m_szChangeAllCitiesDisplayText;
	CvWString m_szPrereqMinDisplayText;
	CvWString m_szPrereqMaxDisplayText;

	int m_aaiChangePropagator[NUM_GAMEOBJECTS][NUM_GAMEOBJECTS]; // from/to, in percent

	std::vector<PropertyBuilding> m_aPropertyBuildings;
	std::vector<PropertyPromotion> m_aPropertyPromotions;
	EraArray m_aTargetLevelbyEraTypes;
};

#endif // CV_PROPERTY_INFO_H
