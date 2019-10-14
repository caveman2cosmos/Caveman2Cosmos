#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvProperties.h
//
//  PURPOSE: Generic properties for Civ4 classes
//
//------------------------------------------------------------------------------------------------
#ifndef CV_PROPERTIES_H
#define CV_PROPERTIES_H

#include "CvXMLLoadUtilityModTools.h"
//#include "CvGameObject.h"

//#include "CvProperty.h"

class CvGame;
class CvTeam;
class CvPlayer;
class CvCity;
class CvUnit;
class CvPlot;
class CvGameObject;

struct PropertyBuilding
{
	int			  iMinValue;
	int			  iMaxValue;
	BuildingTypes eBuilding;
};

struct PropertyPromotion
{
	int			   iMinValue;
	int			   iMaxValue;
	PromotionTypes ePromotion;
};

class CvProperties
{
  public:
	CvProperties();

	explicit CvProperties(CvGame* pGame);
	explicit CvProperties(CvTeam* pTeam);
	explicit CvProperties(CvPlayer* pPlayer);
	explicit CvProperties(CvCity* pCity);
	explicit CvProperties(CvUnit* pUnit);
	explicit CvProperties(CvPlot* pPlot);

	int	 getProperty(int index) const;
	int	 getValue(int index) const;
	int	 getChangeProperty(int index) const;
	int	 getChange(int index) const;
	int	 getNumProperties() const;
	int	 getPositionByProperty(int eProp) const;
	int	 getValueByProperty(int eProp) const;
	int	 getChangeByProperty(int eProp) const;
	void setValue(int index, int iVal);
	void setChange(int index, int iVal);
	void setValueByProperty(int eProp, int iVal);
	void setChangeByProperty(int eProp, int iVal);
	void changeValue(int index, int iChange);
	void changeValueByProperty(int eProp, int iChange);
	void changeChangeByProperty(int eProp, int iChange);
	void propagateChange(int eProp, int iChange);

	void addProperties(const CvProperties* pProp);
	void subtractProperties(const CvProperties* pProp);

	bool isEmpty() const;
	void clear();
	void clearChange();
	void clearForRecalculate();

	//Note: The comparison operators are NOT symmetric. Only properties defined in the second operand are considered.
	// That means any property object is smaller than the empty property and bigger as well.
	bool operator<(const CvProperties& prop) const;
	bool operator<=(const CvProperties& prop) const;
	bool operator>(const CvProperties& prop) const;
	bool operator>=(const CvProperties& prop) const;
	bool operator==(const CvProperties& prop) const;
	bool operator!=(const CvProperties& prop) const;

	void buildRequiresMinString(CvWStringBuffer& szBuffer, const CvProperties* pProp) const;
	void buildRequiresMaxString(CvWStringBuffer& szBuffer, const CvProperties* pProp) const;
	void buildChangesString(CvWStringBuffer& szBuffer, CvWString* szCity = NULL) const;
	void buildCompactChangesString(CvWStringBuffer& szBuffer) const;
	void buildChangesAllCitiesString(CvWStringBuffer& szBuffer) const;
	void buildDisplayString(CvWStringBuffer& szBuffer) const;

	// For Python
	std::wstring getPropertyDisplay(int index) const;

	void read(FDataStreamBase* pStream);
	void readWrapper(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
	void writeWrapper(FDataStreamBase* pStream);
	bool read(CvXMLLoadUtility* pXML, const wchar_t* szTagName = L"Properties");
	void copyNonDefaults(CvProperties* pProp, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

  protected:
	std::vector<std::pair<int, int> > m_aiProperty;
	std::vector<std::pair<int, int> > m_aiPropertyChange;

	// Pointer to the object to which the properties belong
	CvGameObject* m_pGameObject;
};

#endif