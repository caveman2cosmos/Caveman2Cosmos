#pragma once

#ifndef CV_GOODY_INFO_H
#define CV_GOODY_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGoodyInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGoodyInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvGoodyInfo();
	virtual ~CvGoodyInfo();

	int getGold() const;
	int getGoldRand1() const;
	int getGoldRand2() const;
	int getMapOffset() const;
	int getMapRange() const;
	int getMapProb() const;
	int getExperience() const;
	int getHealing() const;
	int getDamagePrereq() const;
	int getBarbarianUnitProb() const;
	int getMinBarbarians() const;
	int getGoodyUnit() const;
	int getBarbarianUnit() const;
	int getEraType() const;
	int getNotEraType() const;
	int getResearch() const;

	bool isTech() const;
	bool isBad() const;
	bool isNaval() const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

	const char* getSound() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvGoodyInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iGold;
	int m_iGoldRand1;
	int m_iGoldRand2;
	int m_iMapOffset;
	int m_iMapRange;
	int m_iMapProb;
	int m_iExperience;
	int m_iHealing;
	int m_iDamagePrereq;
	int m_iBarbarianUnitProb;
	int m_iMinBarbarians;
	int m_iGoodyUnit;
	int m_iBarbarianUnit;
	int m_iEraType;
	int m_iNotEraType;
	int m_iResearch;

	bool m_bTech;
	bool m_bBad;
	bool m_bNaval;

	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;

	CvString m_szSound;
};

#endif // CV_GOODY_INFO_H
