//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvProperties.cpp
//
//  PURPOSE: Generic properties for Civ4 classes
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGame.h"
#include "CvGameObject.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvPlayer.h"
#include "CvPlot.h"
#include "CvProperties.h"
#include "CvTeam.h"
#include "CvUnit.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvProperties::CvProperties()
{
	m_pGameObject = NULL;
}

CvProperties::CvProperties(CvGame* pGame)
{
	m_pGameObject = pGame->getGameObject();
}

CvProperties::CvProperties(CvTeam* pTeam)
{
	m_pGameObject = pTeam->getGameObject();
}

CvProperties::CvProperties(CvPlayer* pPlayer)
{
	m_pGameObject = pPlayer->getGameObject();
}

CvProperties::CvProperties(CvCity* pCity)
{
	m_pGameObject = pCity->getGameObject();
}

CvProperties::CvProperties(CvUnit* pUnit)
{
	m_pGameObject = pUnit->getGameObject();
}

CvProperties::CvProperties(CvPlot* pPlot)
{
	m_pGameObject = pPlot->getGameObject();
}

PropertyTypes CvProperties::getProperty(int index) const
{
	FASSERT_BOUNDS(0, (int)m_aiProperty.size(), index)
	return m_aiProperty[index].prop;
}

int CvProperties::getValue(int index) const
{
	FASSERT_BOUNDS(0, (int)m_aiProperty.size(), index)
	return m_aiProperty[index].value;
}

PropertyTypes CvProperties::getChangeProperty(int index) const
{
	FASSERT_BOUNDS(0, (int)m_aiPropertyChange.size(), index)
	return m_aiPropertyChange[index].prop;
}

int CvProperties::getChange(int index) const
{
	FASSERT_BOUNDS(0, (int)m_aiPropertyChange.size(), index)
	return m_aiPropertyChange[index].value;
}

int CvProperties::getNumProperties() const
{
	return m_aiProperty.size();
}

int CvProperties::getPositionByProperty(PropertyTypes eProp) const
{
	for (prop_value_const_iterator it = m_aiProperty.begin();it!=m_aiProperty.end(); ++it)
	{
		if (it->prop == eProp)
			return it - m_aiProperty.begin();
	}
	return -1;
}

int CvProperties::getValueByProperty(PropertyTypes eProp) const
{
	const int index = getPositionByProperty(eProp);
	if (index < 0)
		return 0;
	else
		return getValue(index);
}

int CvProperties::getChangeByProperty(PropertyTypes eProp) const
{
	foreach_(const PropertyValue& it, m_aiPropertyChange)
	{
		if (it.prop == eProp)
			return it.value;
	}
	return 0;
}

void CvProperties::setChangeByProperty(PropertyTypes eProp, int iVal)
{
	foreach_(PropertyValue& it, m_aiPropertyChange)
	{
		if (it.prop == eProp)
		{
			it.value = iVal;
			return;
		}
	}
	m_aiPropertyChange.push_back(PropertyValue(eProp,iVal));;
}

void CvProperties::changeChangeByProperty(PropertyTypes eProp, int iChange)
{
	foreach_(PropertyValue& it, m_aiPropertyChange)
	{
		if (it.prop == eProp)
		{
			it.value += iChange;
			return;
		}
	}
	m_aiPropertyChange.push_back(PropertyValue(eProp,iChange));;
}

void CvProperties::setValue(int index, int iVal)
{
	//TBOOSHUNTHERE
	//CvString szBuffer;
	//szBuffer.format("SetValue, index %i, iValue %i.", index, iVal);
	//gDLL->logMsg("PropertyBuildingOOS.log", szBuffer.c_str(), false, false);
	FASSERT_BOUNDS(0, (int)m_aiProperty.size(), index)
	const int iOldVal = m_aiProperty[index].value;
	if (iOldVal != iVal)
	{
		m_aiProperty[index].value = iVal;
		if (m_pGameObject)
			m_pGameObject->eventPropertyChanged(m_aiProperty[index].prop, iVal);
		// If this sets the value to 0, remove the property
		//if (iVal == 0)
		//	m_aiProperty.erase(m_aiProperty.begin()+index);
	}
}

void CvProperties::setChange(int index, int iVal)
{
	FASSERT_BOUNDS(0, (int)m_aiPropertyChange.size(), index)
	m_aiPropertyChange[index].value = iVal;
}

void CvProperties::setValueByProperty(PropertyTypes eProp, int iVal)
{
	//TBOOSHUNTHERE
	//CvString szBuffer;
	//szBuffer.format("SetValueByProperty, eProp %i, iValue %i.", eProp, iVal);
	//gDLL->logMsg("PropertyBuildingOOS.log", szBuffer.c_str(), false, false);
	const int index = getPositionByProperty(eProp);
	if (index < 0)
	{
		if (iVal != 0)
		{
			m_aiProperty.push_back(PropertyValue(eProp,iVal));
			if (m_pGameObject)
				m_pGameObject->eventPropertyChanged(eProp, iVal);
		}
	}
	else
		setValue(index, iVal);
}

void CvProperties::changeValue(int index, int iChange)
{
	if (iChange == 0)
		return;

	const PropertyTypes eProperty = getProperty(index);

	setValue(index, getValue(index) + iChange);
	changeChangeByProperty(eProperty, iChange);
	if (m_pGameObject)
	{
		propagateChange(eProperty, iChange);
	}
}

void CvProperties::changeValueByProperty(PropertyTypes eProp, int iChange)
{
	//TBOOSHUNTHERE
	//CvString szBuffer;
	//szBuffer.format("changeValueByProperty, eProp %i, iChange %i.", eProp, iChange);
	//gDLL->logMsg("PropertyBuildingOOS.log", szBuffer.c_str(), false, false);
	if (iChange == 0)
		return;

	const int index = getPositionByProperty(eProp);
	if (index < 0)
	{
		m_aiProperty.push_back(PropertyValue(eProp,iChange));
		changeChangeByProperty(eProp, iChange);
		if (m_pGameObject)
		{
			propagateChange(eProp, iChange);
			m_pGameObject->eventPropertyChanged(eProp, iChange);
		}
	}
	else
		changeValue(index, iChange);
}

// helper function for propagating change
void callChangeValueByProperty(const CvGameObject* pObject, PropertyTypes eProp, int iChange)
{
	pObject->getProperties()->changeValueByProperty(eProp, iChange);
}

void CvProperties::propagateChange(PropertyTypes eProp, int iChange)
{
	const CvPropertyInfo& kInfo = GC.getPropertyInfo(eProp);
	for (int iI = 0; iI < NUM_GAMEOBJECTS; iI++)
	{
		const int iChangePercent = kInfo.getChangePropagator(m_pGameObject->getGameObjectType(), (GameObjectTypes)iI);
		if (iChangePercent)
		{
			const int iPropChange = (iChange * iChangePercent) / 100;
			m_pGameObject->foreach((GameObjectTypes)iI, bind(callChangeValueByProperty, _1, eProp, iPropChange));
		}
	}
}

void CvProperties::addProperties(const CvProperties* pProp)
{
	const int num = pProp->getNumProperties();
	for (int index = 0; index < num; index++)
	{
		changeValueByProperty(pProp->getProperty(index), pProp->getValue(index));
	}
}

void CvProperties::subtractProperties(const CvProperties* pProp)
{
	const int num = pProp->getNumProperties();
	for (int index = 0; index < num; index++)
	{
		changeValueByProperty(pProp->getProperty(index), - pProp->getValue(index));
	}
}

bool CvProperties::isEmpty() const
{
	return m_aiProperty.empty();
}

void CvProperties::clear()
{
	m_aiProperty.clear();
}

void CvProperties::clearChange()
{
	m_aiPropertyChange.clear();
}

bool CvProperties::isNotSourceDrainProperty(const CvProperties::PropertyValue& p)
{
	return !GC.getPropertyInfo(p.prop).isSourceDrain();
}

void CvProperties::clearForRecalculate()
{
	m_aiProperty.erase(std::remove_if(m_aiProperty.begin(), m_aiProperty.end(), isNotSourceDrainProperty), m_aiProperty.end());
}

void CvProperties::read(FDataStreamBase *pStream)
{
	int num;
	int eProp;
	int iVal;

	// This function replaces the current content if any so clear first
	m_aiProperty.clear();
	m_aiPropertyChange.clear();

	pStream->Read(&num);
	for (int i = 0; i < num; i++)
	{
		pStream->Read(&eProp);
		pStream->Read(&iVal);
		// AIAndy: Changed to avoid usage of the methods that trigger property change events
		//setValueByProperty(eProp, iVal);
		if (eProp > -1)
			m_aiProperty.push_back(PropertyValue(static_cast<PropertyTypes>(eProp), iVal));
	}
}

void CvProperties::readWrapper(FDataStreamBase *pStream)
{
	int iPropertyNum = 0;
	int eProp;
	int iVal;

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();
	wrapper.AttachToStream(pStream);

	//WRAPPER_READ_OBJECT_START(wrapper);

	// This function replaces the current content if any so clear first
	m_aiProperty.clear();
	m_aiPropertyChange.clear();

	WRAPPER_READ(wrapper, "CvProperties",&iPropertyNum);
	for (int i = 0; i < iPropertyNum; i++)
	{
		eProp = -1;
		WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvProperties", REMAPPED_CLASS_TYPE_PROPERTIES, &eProp);
		if (eProp == -1)
		{
			// Handle old save game before property remapping
			WRAPPER_READ(wrapper, "CvProperties", &eProp);
			if (eProp == 0) // crime
				eProp = (int)GC.getPROPERTY_CRIME();
			else if (eProp == 1) // flammability
				eProp = (int)GC.getPROPERTY_FLAMMABILITY();
			if (eProp == -1) // removed property
			{
				WRAPPER_SKIP_ELEMENT(wrapper, "CvProperties", iVal, SAVE_VALUE_TYPE_INT);
				continue;
			}
		}
		WRAPPER_READ(wrapper, "CvProperties",&iVal);
		// AIAndy: Changed to avoid usage of the methods that trigger property change events
		//setValueByProperty(eProp, iVal);
		if (eProp > -1)
			m_aiProperty.push_back(PropertyValue(static_cast<PropertyTypes>(eProp), iVal));
	}

	int iPropertyChangeNum = 0;
	WRAPPER_READ(wrapper, "CvProperties",&iPropertyChangeNum);
	for (int i = 0; i < iPropertyChangeNum; i++)
	{
		eProp = -1;
		WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvProperties", REMAPPED_CLASS_TYPE_PROPERTIES, &eProp);
		WRAPPER_READ(wrapper, "CvProperties",&iVal);
		if (eProp > -1)
			m_aiPropertyChange.push_back(PropertyValue(static_cast<PropertyTypes>(eProp), iVal));
	}

	//WRAPPER_READ_OBJECT_END(wrapper);
}

void CvProperties::write(FDataStreamBase *pStream)
{
	const int iPropertyNum = getNumProperties();
	pStream->Write(iPropertyNum);
	for (int i = 0; i < iPropertyNum; i++)
	{
		pStream->Write(getProperty(i));
		pStream->Write(getValue(i));
	}
}

void CvProperties::writeWrapper(FDataStreamBase *pStream)
{
	const int iPropertyNum = getNumProperties();

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();
	wrapper.AttachToStream(pStream);

	//WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvProperties",iPropertyNum);
	for (int i = 0; i < iPropertyNum; i++)
	{
		const int eProp = getProperty(i);
		const int iVal = getValue(i);

		WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvProperties", REMAPPED_CLASS_TYPE_PROPERTIES, eProp);
		WRAPPER_WRITE(wrapper, "CvProperties", iVal);
	}

	const int iPropertyChangeNum = (int)m_aiPropertyChange.size();
	WRAPPER_WRITE(wrapper, "CvProperties", iPropertyChangeNum);
	for (int i = 0; i < iPropertyChangeNum; i++)
	{
		const int eProp = getChangeProperty(i);
		const int iVal = getChange(i);

		WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvProperties", REMAPPED_CLASS_TYPE_PROPERTIES, eProp);
		WRAPPER_WRITE(wrapper, "CvProperties",iVal);
	}

	//WRAPPER_WRITE_OBJECT_END(wrapper);
}

bool CvProperties::read(CvXMLLoadUtility* pXML, const wchar_t* szTagName)
{
	if(pXML->TryMoveToXmlFirstChild(szTagName))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"Property"))
			{
				do
				{
					int iVal;
					CvString szTextVal;
					pXML->GetChildXmlValByName(szTextVal, L"PropertyType");
					const int eProp = pXML->GetInfoClass(szTextVal);
					pXML->GetOptionalChildXmlValByName(&iVal, L"iPropertyValue");
					setValueByProperty(static_cast<PropertyTypes>(eProp), iVal);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvProperties::copyNonDefaults(const CvProperties* pProp)
{
	const int num = pProp->getNumProperties();
	for (int index = 0; index < num; index++)
	{
		if (getPositionByProperty(pProp->getProperty(index)) < 0)
			setValueByProperty(pProp->getProperty(index), pProp->getValue(index));
	}
}

bool CvProperties::operator<(const CvProperties& prop) const
{
	const int num = prop.getNumProperties();
	for (int index = 0; index < num; index++)
	{
		if (getValueByProperty(prop.getProperty(index)) >= prop.getValue(index))
			return false;
	}
	return true;
}

bool CvProperties::operator<=(const CvProperties& prop) const
{
	const int num = prop.getNumProperties();
	for (int index = 0; index < num; index++)
	{
		if (getValueByProperty(prop.getProperty(index)) > prop.getValue(index))
			return false;
	}
	return true;
}

bool CvProperties::operator>(const CvProperties& prop) const
{
	const int num = prop.getNumProperties();
	for (int index = 0; index < num; index++)
	{
		if (getValueByProperty(prop.getProperty(index)) <= prop.getValue(index))
			return false;
	}
	return true;
}

bool CvProperties::operator>=(const CvProperties& prop) const
{
	const int num = prop.getNumProperties();
	for (int index = 0; index < num; index++)
	{
		if (getValueByProperty(prop.getProperty(index)) < prop.getValue(index))
			return false;
	}
	return true;
}

bool CvProperties::operator==(const CvProperties& prop) const
{
	const int num = prop.getNumProperties();
	for (int index = 0; index < num; index++)
	{
		if (getValueByProperty(prop.getProperty(index)) != prop.getValue(index))
			return false;
	}
	return true;
}

bool CvProperties::operator!=(const CvProperties& prop) const
{
	const int num = prop.getNumProperties();
	for (int index = 0; index < num; index++)
	{
		if (getValueByProperty(prop.getProperty(index)) == prop.getValue(index))
			return false;
	}
	return true;
}

void CvProperties::buildRequiresMinString(CvWStringBuffer& szBuffer, const CvProperties* pProp) const
{
	const int num = getNumProperties();
	for (int index = 0; index < num; index++)
	{
		if ((pProp == NULL) || (pProp->getValueByProperty(getProperty(index)) < getValue(index)))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText(GC.getPropertyInfo((PropertyTypes)getProperty(index)).getPrereqMinDisplayText(), getValue(index)));
		}
	}
}

void CvProperties::buildRequiresMaxString(CvWStringBuffer& szBuffer, const CvProperties* pProp) const
{
	const int num = getNumProperties();
	for (int index = 0; index < num; index++)
	{
		if ((pProp == NULL) || (pProp->getValueByProperty(getProperty(index)) > getValue(index)))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText(GC.getPropertyInfo((PropertyTypes)getProperty(index)).getPrereqMaxDisplayText(), getValue(index)));
		}
	}
}

void CvProperties::buildChangesString(CvWStringBuffer& szBuffer, CvWString* pszCity) const
{
	const int num = getNumProperties();
	for (int iI = 0; iI < num; iI++)
	{
		szBuffer.append(NEWLINE);
		if (pszCity)
		{
			szBuffer.append(*pszCity);
			szBuffer.append(": ");
		}
		CvWString szTemp;
		szTemp.Format(L"%c: %+d", GC.getPropertyInfo((PropertyTypes)getProperty(iI)).getChar(), getValue(iI));
		szBuffer.append(szTemp);
		//szBuffer.append(gDLL->getText(GC.getPropertyInfo((PropertyTypes)getProperty(iI)).getChangeDisplayText(), getValue(iI), GC.getPropertyInfo((PropertyTypes)getProperty(iI)).getChar()));
	}
}

void CvProperties::buildCompactChangesString(CvWStringBuffer& szBuffer) const
{
	const int num = getNumProperties();
	for (int iI = 0; iI < num; iI++)
	{
		CvWString szTemp;
		szTemp.Format(L" %d%c", getValue(iI), GC.getPropertyInfo((PropertyTypes)getProperty(iI)).getChar());
		szBuffer.append(szTemp);
	}
}

void CvProperties::buildChangesAllCitiesString(CvWStringBuffer& szBuffer) const
{
	const int num = getNumProperties();
	for (int iI = 0; iI < num; iI++)
	{
		szBuffer.append(NEWLINE);
		CvWString szTemp;
		szTemp.Format(L"%c (All Cities): %+d", GC.getPropertyInfo((PropertyTypes)getProperty(iI)).getChar(), getValue(iI));
		szBuffer.append(szTemp);
		//szBuffer.append(gDLL->getText(GC.getPropertyInfo((PropertyTypes)getProperty(iI)).getChangeAllCitiesDisplayText(), getValue(iI), GC.getPropertyInfo((PropertyTypes)getProperty(iI)).getChar()));
	}
}

void CvProperties::buildDisplayString(CvWStringBuffer& szBuffer) const
{
	const int num = getNumProperties();
	for (int iI = 0; iI < num; iI++)
	{
		szBuffer.append(NEWLINE);
		CvWString szTemp;
		szTemp.Format(L"%c: %+d", GC.getPropertyInfo((PropertyTypes)getProperty(iI)).getChar(), getValue(iI));
		szBuffer.append(szTemp);
		//szBuffer.append(gDLL->getText(GC.getPropertyInfo((PropertyTypes)getProperty(iI)).getValueDisplayText(), getValue(iI), CvWString::format(L"%c", GC.getPropertyInfo((PropertyTypes) iI).getChar())));
	}
}

std::wstring CvProperties::getPropertyDisplay(int index) const
{
	CvWString szTemp;
	if (index < getNumProperties())
		szTemp.Format(L"%c %s", GC.getPropertyInfo((PropertyTypes)getProperty(index)).getChar(), GC.getPropertyInfo((PropertyTypes)getProperty(index)).getText());
	return szTemp.GetCString();
}

void CvProperties::getCheckSum(unsigned int &iSum) const
{
	CheckSumC(iSum, m_aiProperty);
}
