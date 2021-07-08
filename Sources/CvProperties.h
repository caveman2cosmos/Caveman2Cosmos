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

#include "CheckSum.h"
#include "CvEnums.h"

class CvCity;
class CvGame;
class CvGameObject;
class CvPlayer;
class CvPlot;
class CvTeam;
class CvUnit;
class CvXMLLoadUtility;

struct PropertyBuilding
{
	int iMinValue;
	int iMaxValue;
	BuildingTypes eBuilding;
};

struct PropertyPromotion
{
	int iMinValue;
	int iMaxValue;
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

	PropertyTypes getProperty(int index) const;

	int getValue(int index) const;
	PropertyTypes getChangeProperty(int index) const;
	int getChange(int index) const;
	int getNumProperties() const;
	int getPositionByProperty(PropertyTypes eProp) const;
	int getValueByProperty(PropertyTypes eProp) const;
	int getChangeByProperty(PropertyTypes eProp) const;
	void setValue(int index, int iVal);
	void setChange(int index, int iVal);
	void setValueByProperty(PropertyTypes eProp, int iVal);
	void setChangeByProperty(PropertyTypes eProp, int iVal);
	void changeValue(int index, int iChange);
	void changeValueByProperty(PropertyTypes eProp, int iChange);
	void changeChangeByProperty(PropertyTypes eProp, int iChange);
	void propagateChange(PropertyTypes eProp, int iChange);

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
	void copyNonDefaults(const CvProperties* pProp);

	void getCheckSum(uint32_t& iSum) const;

private:
	struct PropertyValue
	{
		PropertyValue(PropertyTypes prop = NO_PROPERTY, int value = 0) : prop(prop), value(value) {}
		PropertyTypes prop;
		int value;
		friend inline void CheckSum(uint32_t& iSum, const PropertyValue& propValue)
		{
			CheckSum(iSum, propValue.prop);
			CheckSum(iSum, propValue.value);
		}
	};




	static bool isNotSourceDrainProperty(const CvProperties::PropertyValue& p);

	typedef std::vector<PropertyValue> PropertyValueVector;
	typedef PropertyValueVector::const_iterator prop_value_const_iterator;
	typedef PropertyValueVector::iterator prop_value_iterator;

	PropertyValueVector m_aiProperty;
	PropertyValueVector m_aiPropertyChange;

	// Pointer to the object to which the properties belong
	CvGameObject* m_pGameObject;
};

#endif
