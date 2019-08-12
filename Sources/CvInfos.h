#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvInfos.h
//
//  PURPOSE: All Civ4 info classes and the base class for them
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef CV_INFO_H
#define CV_INFO_H

#include "CvXMLLoadUtilityModTools.h"
#include "CvProperties.h"
#include "CvPropertySource.h"
#include "CvPropertyInteraction.h"
#include "CvPropertyPropagator.h"
#include "CvPropertyManipulators.h"
#include "CvDate.h"
#include "BoolExpr.h"
#include "IntExpr.h"
#include "IDValueMap.h"
#include "CheckSum.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class
#pragma warning( disable: 4127 )


extern bool shouldHaveType;

class CvXMLLoadUtility;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInfoBase
//
//  DESC:   The base class for all info classes to inherit from.  This gives us
//			the base description and type strings
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:
	CvInfoBase();
	CvInfoBase(const char* szType);
	DllExport virtual ~CvInfoBase();

	virtual void reset();

	bool isGraphicalOnly() const;						// Exposed to Python

	DllExport const TCHAR* getType() const;				// Exposed to Python
	virtual const TCHAR* getButton() const;				// Exposed to Python

	const wchar* getCivilopediaKey() const;
	const wchar* getHelpKey() const;
	const wchar* getStrategyKey() const;
	//const wchar* getGenericCategoryKey() const;

	// for python wide string handling
	std::wstring pyGetTextKey() { return getTextKeyWide(); }				// Exposed to Python
	std::wstring pyGetDescription() { return getDescription(0); }			// Exposed to Python
	std::wstring pyGetDescriptionForm(uint uiForm) { return getDescription(uiForm); }	// Exposed to Python
	std::wstring pyGetText() { return getText(); }							// Exposed to Python
	std::wstring pyGetCivilopedia() { return getCivilopedia(); }			// Exposed to Python
	std::wstring pyGetHelp() { return getHelp(); }							// Exposed to Python
	std::wstring pyGetStrategy() { return getStrategy(); }					// Exposed to Python
/*	std::wstring pyGetGenericCategory() { return getGenericCategory(); }*/	// Exposed to Python

	DllExport const wchar* getTextKeyWide() const;
	DllExport const wchar* getDescription(uint uiForm = 0) const;
	DllExport const wchar* getText() const;
	const wchar* getCivilopedia() const;
	DllExport const wchar* getHelp() const;
	const wchar* getStrategy() const;
/*	const wchar* getGenericCategory() const;*/

	bool isMatchForLink(std::wstring szLink, bool bKeysOnly) const;

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual bool readPass2(CvXMLLoadUtility* pXML) { pXML; FAssertMsg(false, "Override this"); return false; }
	virtual bool readPass3() { FAssertMsg(false, "Override this"); return false; }
	virtual void copyNonDefaults(CvInfoBase* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL );
	virtual void copyNonDefaultsReadPass2(CvInfoBase* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL, bool bOver = false)
	{ /* AIAndy: Default implementation for full copy of info without knowledge of one/twopass */ }
	virtual void getCheckSum(unsigned int& iSum) { };
	virtual const wchar* getExtraHoverText() const { return L""; };

protected:

	bool doneReadingXML(CvXMLLoadUtility* pXML);

	bool m_bGraphicalOnly;
	CvString m_szType;
	CvString m_szButton;	// Used for Infos that don't require an ArtAssetInfo
	CvWString m_szTextKey;
	CvWString m_szCivilopediaKey;
	CvWString m_szHelpKey;
	CvWString m_szStrategyKey;/*
	CvWString m_szGenericCategoryKey;*/


	// translated text
	std::vector<CvString> m_aszExtraXMLforPass3;
	mutable std::vector<CvWString> m_aCachedDescriptions;
	mutable CvWString m_szCachedText;
	mutable CvWString m_szCachedHelp;
	mutable CvWString m_szCachedStrategy;
	mutable CvWString m_szCachedCivilopedia;/*
	mutable CvWString m_szCachedGenericCategory;*/
};

//
// holds the scale for scalable objects
//
class CvScalableInfo
{
public:

	CvScalableInfo() : m_fScale(1.0f), m_fInterfaceScale(1.0f) { }

	DllExport float getScale() const;
	void setScale(float fScale);

	DllExport float getInterfaceScale() const;
	void setInterfaceScale(float fInterfaceScale);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvScalableInfo* pClassInfo, CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

protected:

	float m_fScale;			// Exposed to Python
	float m_fInterfaceScale;	//!< the scale of the unit appearing in the interface screens
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:	  CvHotkeyInfo
//!  \brief			holds the hotkey info for an info class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHotkeyInfo : public CvInfoBase
{
public:
	//constructor
	CvHotkeyInfo();
	//destructor
	virtual ~CvHotkeyInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvHotkeyInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	int getActionInfoIndex() const;
	void setActionInfoIndex(int i);

	int getHotKeyVal() const;
	void setHotKeyVal(int i);
	int getHotKeyPriority() const;
	void setHotKeyPriority(int i);
	int getHotKeyValAlt() const;
	void setHotKeyValAlt(int i);
	int getHotKeyPriorityAlt() const;
	void setHotKeyPriorityAlt(int i);
	int getOrderPriority() const;
	void setOrderPriority(int i);

	bool isAltDown() const;
	void setAltDown(bool b);
	bool isShiftDown() const;
	void setShiftDown(bool b);
	bool isCtrlDown() const;
	void setCtrlDown(bool b);
	bool isAltDownAlt() const;
	void setAltDownAlt(bool b);
	bool isShiftDownAlt() const;
	void setShiftDownAlt(bool b);
	bool isCtrlDownAlt() const;
	void setCtrlDownAlt(bool b);
	const TCHAR* getHotKey() const;			// Exposed to Python
	void setHotKey(const TCHAR* szVal);
	const WCHAR* getHotKeyDescriptionKey() const;
	const WCHAR* getHotKeyAltDescriptionKey() const;
	const WCHAR* getHotKeyString() const;

	std::wstring getHotKeyDescription() const;
	void setHotKeyDescription(const wchar* szHotKeyDescKey, const wchar* szHotKeyAltDescKey, const wchar* szHotKeyString);

protected:

	int m_iActionInfoIndex;

	int m_iHotKeyVal;
	int m_iHotKeyPriority;
	int m_iHotKeyValAlt;
	int m_iHotKeyPriorityAlt;
	int m_iOrderPriority;

	bool m_bAltDown;
	bool m_bShiftDown;
	bool m_bCtrlDown;
	bool m_bAltDownAlt;
	bool m_bShiftDownAlt;
	bool m_bCtrlDownAlt;

	CvString m_szHotKey;
	CvWString m_szHotKeyDescriptionKey;
	CvWString m_szHotKeyAltDescriptionKey;
	CvWString m_szHotKeyString;
};

#include "CvInfoTerrain.h"
#include "CvInfoGame.h"
#include "CvInfoPlayer.h"
#include "CvInfoBuilding.h"
#include "CvInfoCity.h"
#include "CvInfoUnitCombat.h"
#include "CvInfoUnit.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvControlInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvControlInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvControlInfo();
	virtual ~CvControlInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvControlInfo* pClassInfo, CvXMLLoadUtility* pXML);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvActionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvActionInfo
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvActionInfo();
	virtual ~CvActionInfo();

	int getMissionData() const;			// Exposed to Python
	int getCommandData() const;			// Exposed to Python

	int getAutomateType() const;
	int getInterfaceModeType() const;	// Exposed to Python
	int getMissionType() const;			// Exposed to Python
	int getCommandType() const;			// Exposed to Python
	int getControlType() const;			// Exposed to Python
	int getOriginalIndex() const;
	void setOriginalIndex(int i);

	bool isConfirmCommand() const;		// Exposed to Python
	DllExport bool isVisible() const;	// Exposed to Python
	DllExport ActionSubTypes getSubType() const;
	void setSubType(ActionSubTypes eSubType);

	// functions to replace the CvInfoBase calls
	const TCHAR* getType() const;
	const wchar* getDescription() const;
	const wchar* getCivilopedia() const;
	const wchar* getHelp() const;
	const wchar* getStrategy() const;/*
	const wchar* getGenericCategory() const;*/
	virtual const TCHAR* getButton() const;
	const wchar* getTextKeyWide() const;

	// functions to replace the CvHotkey calls
	int getActionInfoIndex() const;
	DllExport int getHotKeyVal() const;
	DllExport int getHotKeyPriority() const;
	DllExport int getHotKeyValAlt() const;
	DllExport int getHotKeyPriorityAlt() const;
	int getOrderPriority() const;

	DllExport bool isAltDown() const;
	DllExport bool isShiftDown() const;
	DllExport bool isCtrlDown() const;
	DllExport bool isAltDownAlt() const;
	DllExport bool isShiftDownAlt() const;
	DllExport bool isCtrlDownAlt() const;

	const TCHAR* getHotKey() const;				// Exposed to Python

	std::wstring getHotKeyDescription() const;	// Exposed to Python

//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iOriginalIndex;
	ActionSubTypes m_eSubType;

private:
	CvHotkeyInfo* getHotkeyInfo() const;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInterfaceModeInfo (ADD to Python)
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInterfaceModeInfo :
	public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvInterfaceModeInfo();
	virtual ~CvInterfaceModeInfo();

	DllExport int getCursorIndex() const;
	DllExport int getMissionType() const;

	bool getVisible() const;
	DllExport bool getGotoPlot() const;
	bool getHighlightPlot() const;
	bool getSelectType() const;
	bool getSelectAll() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvInterfaceModeInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iCursorIndex;
	int m_iMissionType;

	bool m_bVisible;
	bool m_bGotoPlot;
	bool m_bHighlightPlot;
	bool m_bSelectType;
	bool m_bSelectAll;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAdvisorInfo (ADD to Python)
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAdvisorInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvAdvisorInfo();
	virtual ~CvAdvisorInfo();

	const TCHAR* getTexture() const;		// Exposed to Python
	void setTexture(const TCHAR* szVal);
	int getNumCodes() const;
	int getEnableCode(uint uiCode) const;
	int getDisableCode(uint uiCode) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvAdvisorInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	CvString m_szTexture;
	std::vector< std::pair< int, int > > m_vctEnableDisableCodes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCursorInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCursorInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCursorInfo();
	virtual ~CvCursorInfo();

	DllExport const TCHAR* getPath();		// Exposed to Python
	void setPath(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvCursorInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szPath;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvThroneRoomCamera
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThroneRoomCamera : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvThroneRoomCamera();
	virtual ~CvThroneRoomCamera();

	DllExport const TCHAR* getFileName();
	void setFileName(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvThroneRoomCamera* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szFileName;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvThroneRoomInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThroneRoomInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvThroneRoomInfo();
	virtual ~CvThroneRoomInfo();

	DllExport const TCHAR* getEvent();
	void setEvent(const TCHAR* szVal);
	DllExport const TCHAR* getNodeName();
	void setNodeName(const TCHAR* szVal);
	DllExport int getFromState();
	void setFromState(int iVal);
	DllExport int getToState();
	void setToState(int iVal);
	DllExport int getAnimation();
	void setAnimation(int iVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvThroneRoomInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iFromState;
	int m_iToState;
	int m_iAnimation;
	CvString m_szEvent;
	CvString m_szNodeName;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvThroneRoomStyleInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThroneRoomStyleInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvThroneRoomStyleInfo();
	virtual ~CvThroneRoomStyleInfo();

	DllExport const TCHAR* getArtStyleType();
	void setArtStyleType(const TCHAR* szVal);
	DllExport const TCHAR* getEraType();
	void setEraType(const TCHAR* szVal);
	DllExport const TCHAR* getFileName();
	void setFileName(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvThroneRoomStyleInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szArtStyleType;
	CvString m_szEraType;
	CvString m_szFileName;
	std::vector<CvString> m_aNodeNames;
	std::vector<CvString> m_aTextureNames;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSlideShowInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSlideShowInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSlideShowInfo();
	virtual ~CvSlideShowInfo();

	DllExport const TCHAR* getPath();
	void setPath(const TCHAR* szVal);
	DllExport const TCHAR* getTransitionType();
	void setTransitionType(const TCHAR* szVal);
	DllExport float getStartTime();
	void setStartTime(float fVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSlideShowInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	float m_fStartTime;
	CvString m_szPath;
	CvString m_szTransitionType;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSlideShowRandomInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSlideShowRandomInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSlideShowRandomInfo();
	virtual ~CvSlideShowRandomInfo();

	DllExport const TCHAR* getPath();
	void setPath(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSlideShowRandomInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szPath;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvWorldPickerInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWorldPickerInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvWorldPickerInfo();
	virtual ~CvWorldPickerInfo();

	DllExport const TCHAR* getMapName();
	void setMapName(const TCHAR* szVal);
	DllExport const TCHAR* getModelFile();
	void setModelFile(const TCHAR* szVal);
	DllExport int getNumSizes();
	DllExport float getSize(int index);
	DllExport int getNumClimates();
	DllExport const TCHAR* getClimatePath(int index);
	DllExport int getNumWaterLevelDecals();
	DllExport const TCHAR* getWaterLevelDecalPath(int index);
	DllExport int getNumWaterLevelGloss();
	DllExport const TCHAR* getWaterLevelGlossPath(int index);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvWorldPickerInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szMapName;
	CvString m_szModelFile;
	std::vector<float> m_aSizes;
	std::vector<CvString> m_aClimates;
	std::vector<CvString> m_aWaterLevelDecals;
	std::vector<CvString> m_aWaterLevelGloss;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAnimationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef std::vector<std::pair<int,float> > CvAnimationPathDefinition;
typedef std::pair<int,int >			CvAnimationCategoryDefinition;

class CvAnimationPathInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
	public:

		CvAnimationPathInfo();
		virtual ~CvAnimationPathInfo();

		DllExport int getPathCategory( int i );
		float getPathParameter( int i );
		DllExport int getNumPathDefinitions();
		DllExport CvAnimationPathDefinition * getPath( );
		DllExport bool isMissionPath() const;

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvAnimationPathInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:

		CvAnimationPathDefinition 	m_vctPathDefinition;	//!< Animation path definitions, pair(category,param).
		bool						m_bMissionPath;			//!< True if this animation is used in missions
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAnimationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAnimationCategoryInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
	public:

		CvAnimationCategoryInfo();
		virtual ~CvAnimationCategoryInfo();

		DllExport int getCategoryBaseID( );
		DllExport int getCategoryDefaultTo( );

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvAnimationCategoryInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:

		CvAnimationCategoryDefinition	m_kCategory;		//!< The pair(base IDs, default categories) defining the animation categories
		CvString						m_szDefaultTo;		//!< Holds the default to parameter, until all categories are read
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEntityEventInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEntityEventInfo : public CvInfoBase
{
		//---------------------------PUBLIC INTERFACE---------------------------------
	public:

		CvEntityEventInfo();
		virtual ~CvEntityEventInfo();

		// serialization
		void read(FDataStreamBase* pStream) {}
		void write(FDataStreamBase* pStream) {}

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvEntityEventInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

		DllExport AnimationPathTypes getAnimationPathType(int iIndex = 0) const;
		DllExport EffectTypes getEffectType(int iIndex = 0) const;
		int getAnimationPathCount() const;
		int getEffectTypeCount() const;

		bool getUpdateFormation() const;

		//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:

		std::vector<AnimationPathTypes>	m_vctAnimationPathType;
		std::vector<EffectTypes>		m_vctEffectTypes;
		bool							m_bUpdateFormation;
};

// The below classes are for the ArtFile Management
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  classes : CvArtInfos
//
// This is also an abstract BASE class
//
//  DESC:  Used to store data from Art\Civ4ArtDefines.xml
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAssetInfoBase : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvAssetInfoBase()  {}
	virtual ~CvAssetInfoBase() {}

	const TCHAR* getTag() const;				// Exposed to Python
	void setTag(const TCHAR* szDesc);			// Exposed to Python

	DllExport const TCHAR* getPath() const;		// Exposed to Python
	void setPath(const TCHAR* szDesc);			// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvAssetInfoBase* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szPath;

};

class CvArtInfoAsset : 	public CvAssetInfoBase
{
public:

	CvArtInfoAsset() {}
	virtual ~CvArtInfoAsset() {}

	DllExport const TCHAR* getNIF() const;		// Exposed to Python
	DllExport const TCHAR* getKFM() const;		// Exposed to Python

	void setNIF(const TCHAR* szDesc);			// Exposed to Python
	void setKFM(const TCHAR* szDesc);			// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvArtInfoAsset* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szKFM;
	CvString m_szNIF;
};

//
//////////////////////////////////////////////////////////////////////////
// Another base class
//////////////////////////////////////////////////////////////////////////

class CvArtInfoScalableAsset :
	public CvArtInfoAsset,
	public CvScalableInfo
{
public:

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvArtInfoScalableAsset* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

};

// todoJS: Remove empty classes if additional items are not added

class CvArtInfoInterface : 	public CvArtInfoAsset
{
public:

	CvArtInfoInterface() {}
	virtual ~CvArtInfoInterface() {}

};

class CvArtInfoMisc : 	public CvArtInfoScalableAsset
{
public:

	CvArtInfoMisc() {}
	virtual ~CvArtInfoMisc() {}

};

class CvArtInfoMovie : 	public CvArtInfoAsset
{
public:

	CvArtInfoMovie() {}
	virtual ~CvArtInfoMovie() {}

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoMovie* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
};

class CvArtInfoUnit : public CvArtInfoScalableAsset
{
public:

	CvArtInfoUnit();
	virtual ~CvArtInfoUnit();

	DllExport bool getActAsRanged() const;
	DllExport bool getActAsLand() const;
	DllExport bool getActAsAir() const;

	DllExport const TCHAR* getShaderNIF() const;
	void setShaderNIF(const TCHAR* szDesc);

	DllExport const TCHAR* getShadowNIF() const;
	DllExport float getShadowScale() const;
	DllExport const TCHAR* getShadowAttachNode() const;
	DllExport int getDamageStates() const;

	DllExport const TCHAR* getTrailTexture() const;
	DllExport float getTrailWidth() const;
	DllExport float getTrailLength() const;
	DllExport float getTrailTaper() const;
	DllExport float getTrailFadeStarTime() const;
	DllExport float getTrailFadeFalloff() const;

	DllExport float getBattleDistance() const;
	DllExport float getRangedDeathTime() const;
	DllExport float getExchangeAngle() const;
	DllExport bool getCombatExempt() const;
	bool getSmoothMove() const;
	float getAngleInterpRate() const;
	DllExport float getBankRate() const;

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoUnit* pClassInfo, CvXMLLoadUtility* pXML);

	const TCHAR* getTrainSound() const;
	void setTrainSound(const TCHAR* szVal);
	DllExport int getRunLoopSoundTag() const;
	DllExport int getRunEndSoundTag() const;
	DllExport int getPatrolSoundTag() const;
	int getSelectionSoundScriptId() const;
	int getActionSoundScriptId() const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	CvString m_szShaderNIF;		//!< The NIF used if the graphics card supports shaders
	CvString m_szShadowNIF;		//!< The shadow blob NIF to use for the unit
	CvString m_szShadowAttach;	//!< The name of the node to which the shadow takes its x,y position

	float m_fShadowScale;		//!< the scale of the unit's shadow.

	int m_iDamageStates;		//!< The maximum number of damage states this unit type supports
	bool m_bActAsRanged;		//!< true if the unit acts as a ranged unit in combat (but may or may not be actually a ranged unit)
	bool m_bActAsLand;
	bool m_bActAsAir;
	bool m_bCombatExempt;		//!< true if the unit is 'exempt' from combat - ie. it just flees instead of dying
	bool m_bSmoothMove;			//!< true if the unit should do non-linear interpolation for moves

	CvString m_szTrailTexture;	//!< The trail texture of the unit
	float m_fTrailWidth;		//!< The width of the trail
	float m_fTrailLength;		//!< The length of the trail
	float m_fTrailTaper;		//!< Tapering of the trail
	float m_fTrailFadeStartTime;//!< Time after which the trail starts to fade
	float m_fTrailFadeFalloff;	//!< Speed at which the fade happens

	float m_fBattleDistance;	//!< The preferred attack distance of this unit (1.0 == plot size)
	float m_fRangedDeathTime;	//!< The offset from firing in which an opponent should die
	float m_fExchangeAngle;		//!< The angle at which the unit does combat.
	float m_fAngleInterRate;	//!< The rate at which the units' angle interpolates
	float m_fBankRate;

	CvString m_szTrainSound;
	int m_iRunLoopSoundTag;
	int m_iRunEndSoundTag;
	int m_iPatrolSoundTag;
	int m_iSelectionSoundScriptId;
	int m_iActionSoundScriptId;
};

class CvArtInfoBuilding : public CvArtInfoScalableAsset
{
public:

	CvArtInfoBuilding();
	virtual ~CvArtInfoBuilding();

	bool isAnimated() const;				// Exposed to Python
	DllExport const TCHAR* getLSystemName() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvArtInfoBuilding* pClassInfo, CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

protected:

	bool m_bAnimated;
	CvString m_szLSystemName;

};

class CvArtInfoCivilization : public CvArtInfoAsset
{
public:

	CvArtInfoCivilization();
	virtual ~CvArtInfoCivilization();

	bool isWhiteFlag() const;				// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoCivilization* pClassInfo, CvXMLLoadUtility* pXML);

protected:

	bool m_bWhiteFlag;

};

class CvArtInfoLeaderhead : public CvArtInfoAsset
{
public:

	CvArtInfoLeaderhead() {}
	virtual ~CvArtInfoLeaderhead() {}

	DllExport const TCHAR* getNoShaderNIF() const;
	void setNoShaderNIF(const TCHAR* szNIF);
	DllExport const TCHAR* getBackgroundKFM() const;
	void setBackgroundKFM( const TCHAR* szKFM);

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoLeaderhead* pClassInfo, CvXMLLoadUtility* pXML);

protected:

	CvString m_szNoShaderNIF;
	CvString m_szBackgroundKFM;
};

class CvArtInfoBonus : public CvArtInfoScalableAsset
{
public:
	CvArtInfoBonus();
	virtual ~CvArtInfoBonus() {}

	int getFontButtonIndex() const;

	DllExport const TCHAR* getShaderNIF() const;
	void setShaderNIF(const TCHAR* szDesc);

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoBonus* pClassInfo, CvXMLLoadUtility* pXML);

protected:
	CvString m_szShaderNIF;		//!< The NIF used if the graphics card supports shaders
	int m_iFontButtonIndex;
};

class CvArtInfoImprovement : public CvArtInfoScalableAsset
{
public:

	CvArtInfoImprovement();
	virtual ~CvArtInfoImprovement();

	DllExport const TCHAR* getShaderNIF() const;
	void setShaderNIF(const TCHAR* szDesc);

	bool isExtraAnimations() const;				// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoImprovement* pClassInfo, CvXMLLoadUtility* pXML);

protected:
	CvString m_szShaderNIF;		//!< The NIF used if the graphics card supports shaders

	bool m_bExtraAnimations;

};

typedef std::vector<std::pair<int, int> > CvTextureBlendSlotList;

class CvArtInfoTerrain : public CvArtInfoAsset
{
public:

	CvArtInfoTerrain();
	virtual ~CvArtInfoTerrain();

	DllExport const TCHAR* getBaseTexture();
	void setBaseTexture(const TCHAR* szTmp );
	DllExport const TCHAR* getGridTexture();
	void setGridTexture(const TCHAR* szTmp );
	DllExport const TCHAR* getDetailTexture();
	void setDetailTexture(const TCHAR* szTmp);
	DllExport int getLayerOrder();
	DllExport bool useAlphaShader();
	DllExport CvTextureBlendSlotList &getBlendList(int blendMask);

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoTerrain* pClassInfo, CvXMLLoadUtility* pXML);

protected:

	CvString m_szDetailTexture;				//!< Detail texture associated with the Terrain base texture
	CvString m_szGridTexture;

	int m_iLayerOrder;						//!< Layering order of texture
	bool m_bAlphaShader;
	int m_numTextureBlends;						//!< number to blend textures.
	CvTextureBlendSlotList  **m_pTextureSlots;	//!< Array of Textureslots per blend tile
	CvString**	m_pSlotNames;
};

class CvArtInfoFeature : public CvArtInfoScalableAsset
{
public:

	CvArtInfoFeature();
	virtual ~CvArtInfoFeature();

	DllExport bool isAnimated() const;			// Exposed to Python
	DllExport bool isRiverArt() const;			// Exposed to Python
	DllExport TileArtTypes getTileArtType() const;
	DllExport LightTypes getLightType() const;

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void dump();

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvArtInfoFeature* pClassInfo, CvXMLLoadUtility* pXML);

	class FeatureArtModel
	{
	public:
		FeatureArtModel(const CvString &modelFile, RotationTypes rotation)
		{
			m_szModelFile = modelFile;
			m_eRotation = rotation;
		}

		const CvString &getModelFile() const
		{
			return m_szModelFile;
		}

		RotationTypes getRotation() const
		{
			return m_eRotation;
		}

	private:
		CvString m_szModelFile;
		RotationTypes m_eRotation;
	};

	class FeatureArtPiece
	{
	public:
		FeatureArtPiece(int connectionMask)
		{
			m_iConnectionMask = connectionMask;
		}

		int getConnectionMask() const
		{
			return m_iConnectionMask;
		}

		int getNumArtModels() const
		{
			return m_aArtModels.size();
		}

		const FeatureArtModel &getArtModel(int index) const
		{
			FAssertMsg((index >= 0) && (index < (int) m_aArtModels.size()), "[Jason] Invalid feature model file index.");
			return m_aArtModels[index];
		}

	private:
		std::vector<FeatureArtModel> m_aArtModels;
		int m_iConnectionMask;

		friend CvArtInfoFeature;
	};

	class FeatureDummyNode
	{
	public:
		FeatureDummyNode(const CvString &tagName, const CvString &nodeName)
		{
			m_szTag = tagName;
			m_szName = nodeName;
		}

		const CvString getTagName() const
		{
			return m_szTag;
		}

		const CvString getNodeName() const
		{
			return m_szName;
		}

	private:
		CvString m_szTag;
		CvString m_szName;
	};

	class FeatureVariety
	{
	public:
		FeatureVariety()
		{
		}

		const CvString &getVarietyButton() const
		{
			return m_szVarietyButton;
		}

		const FeatureArtPiece &getFeatureArtPiece(int index) const
		{
			FAssertMsg((index >= 0) && (index < (int) m_aFeatureArtPieces.size()), "[Jason] Invalid feature art index.");
			return m_aFeatureArtPieces[index];
		}

		const FeatureArtPiece &getFeatureArtPieceFromConnectionMask(int connectionMask) const
		{
			for(int i=0;i<(int)m_aFeatureArtPieces.size();i++)
				if(m_aFeatureArtPieces[i].getConnectionMask() == connectionMask)
					return m_aFeatureArtPieces[i];

			FAssertMsg(false, "[Jason] Failed to find feature art piece with valid connection mask.");
			return m_aFeatureArtPieces[0];
		}

		const CvString getFeatureDummyNodeName(const CvString &tagName) const
		{
			for(int i=0;i<(int)m_aFeatureDummyNodes.size();i++)
			{
				if(m_aFeatureDummyNodes[i].getTagName().CompareNoCase(tagName) == 0)
					return m_aFeatureDummyNodes[i].getNodeName();
			}

			FAssertMsg(false, "[Jason] Failed to find dummy tag name.");
			return "";
		}

		const CvString getFeatureDummyTag(const CvString &nodeName) const
		{
			for(int i=0;i<(int)m_aFeatureDummyNodes.size();i++)
			{
				if(m_aFeatureDummyNodes[i].getNodeName().CompareNoCase(nodeName) == 0)
					return m_aFeatureDummyNodes[i].getTagName();
			}

			return "";
		}

		FeatureArtPiece &createFeatureArtPieceFromConnectionMask(int connectionMask)
		{
			for(int i=0;i<(int)m_aFeatureArtPieces.size();i++)
				if(m_aFeatureArtPieces[i].getConnectionMask() == connectionMask)
					return m_aFeatureArtPieces[i];

			m_aFeatureArtPieces.push_back(FeatureArtPiece(connectionMask));
			return m_aFeatureArtPieces.back();
		}

		void createFeatureDummyNode(const CvString &tagName, const CvString &nodeName)
		{
			m_aFeatureDummyNodes.push_back(FeatureDummyNode(tagName, nodeName));
		}

	protected:
		std::vector<FeatureArtPiece> m_aFeatureArtPieces;
		std::vector<FeatureDummyNode> m_aFeatureDummyNodes;
		CvString m_szVarietyButton;

		friend CvArtInfoFeature;
	};

	DllExport const FeatureVariety &getVariety(int index) const;
	DllExport int getNumVarieties() const;
	std::string getFeatureDummyNodeName(int variety, std::string tagName);

protected:

	int getConnectionMaskFromString(const CvString &connectionString);
	int getRotatedConnectionMask(int connectionMask, RotationTypes rotation);

	bool m_bAnimated;
	bool m_bRiverArt;
	TileArtTypes m_eTileArtType;
	LightTypes m_eLightType;
	std::vector<FeatureVariety> m_aFeatureVarieties;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvColorInfo
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvColorInfo :
	public CvInfoBase
{
public:

	CvColorInfo();
	virtual ~CvColorInfo();

	DllExport const NiColorA& getColor() const;

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvColorInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

protected:

	NiColorA m_Color;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPlayerColorInfo (ADD to Python)
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerColorInfo :
	public CvInfoBase
{
public:

	CvPlayerColorInfo();
	virtual ~CvPlayerColorInfo();

	DllExport int getColorTypePrimary() const;
	DllExport int getColorTypeSecondary() const;
	int getTextColorType() const;

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvPlayerColorInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

protected:

	int m_iColorTypePrimary;
	int m_iColorTypeSecondary;
	int m_iTextColorType;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvLandscapeInfo
//
//  Purpose:	This info acts as the Civ4Terrain.ini and is initialize in CvXmlLoadUtility with the infos in
//					XML/Terrain/TerrainSettings.xml
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLandscapeInfo :
	public CvInfoBase
{
	public:

		CvLandscapeInfo();
		virtual ~CvLandscapeInfo() {}

		int getFogR() const;
		int getFogG() const;
		int getFogB() const;
		DllExport int getHorizontalGameCell() const;
		DllExport int getVerticalGameCell() const;
		DllExport int getPlotsPerCellX() const;
		DllExport int getPlotsPerCellY() const;
		DllExport int getHorizontalVertCnt() const;
		DllExport int getVerticalVertCnt() const;
		DllExport int getWaterHeight() const;

		float getTextureScaleX() const;
		float getTextureScaleY() const;
		DllExport float getZScale() const;

		bool isUseTerrainShader() const;
		bool isUseLightmap() const;
		bool isRandomMap() const;
		DllExport float getPeakScale() const;
		DllExport float getHillScale() const;

		const TCHAR* getSkyArt();
		void setSkyArt(const TCHAR* szPath);
		const TCHAR* getHeightMap();
		void setHeightMap(const TCHAR* szPath);
		const TCHAR* getTerrainMap();
		void setTerrainMap(const TCHAR* szPath);
		const TCHAR* getNormalMap();
		void setNormalMap(const TCHAR* szPath);
		const TCHAR* getBlendMap();
		void setBlendMap(const TCHAR* szPath);

		void read(FDataStreamBase* stream) {}
		void write(FDataStreamBase* stream) {}

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvLandscapeInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	protected:

		int m_iFogR;
		int m_iFogG;
		int m_iFogB;
		int m_iHorizontalGameCell;
		int m_iVerticalGameCell;
		int m_iPlotsPerCellX;
		int m_iPlotsPerCellY;
		int m_iHorizontalVertCnt;
		int m_iVerticalVertCnt;
		int m_iWaterHeight;

		float m_fTextureScaleX;
		float m_fTextureScaleY;
		float m_fZScale;

		float m_fPeakScale;
		float m_fHillScale;

		bool m_bUseTerrainShader;
		bool m_bUseLightmap;
		bool m_bRandomMap;

		CvString m_szSkyArt;
		CvString m_szHeightMap;
		CvString m_szTerrainMap;
		CvString m_szNormalMap;
		CvString m_szBlendMap;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEffectInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEffectInfo :
	public CvInfoBase,
	public CvScalableInfo
{
public:

	CvEffectInfo();
	virtual ~CvEffectInfo();

	DllExport const TCHAR* getPath() const { return m_szPath; }
	void setPath(const TCHAR* szVal) { m_szPath = szVal; }
	float getUpdateRate( ) const { return m_fUpdateRate; };
	void setUpdateRate( float fUpdateRate ) { m_fUpdateRate = fUpdateRate; }
	bool isProjectile() const { return m_bProjectile; };
	float getProjectileSpeed() const { return m_fProjectileSpeed; };
	float getProjectileArc() const { return m_fProjectileArc; };
	bool isSticky() const { return m_bSticky; };
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvEffectInfo* pClassInfo, CvXMLLoadUtility* pXML);

/************************************************************************************************/
/* RevolutionDCM					  Start		 05/31/10						Afforess	   */
/*																							  */
/* Battle Effects																			   */
/************************************************************************************************/
	bool isBattleEffect() const;
/************************************************************************************************/
/* RevolutionDCM				 Battle Effects END											 */
/************************************************************************************************/
	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

private:
	CvString m_szPath;
	float m_fUpdateRate;
	bool m_bProjectile;
	bool m_bSticky;
	float m_fProjectileSpeed;
	float m_fProjectileArc;

/************************************************************************************************/
/* RevolutionDCM					  Start		 05/31/10						Afforess	   */
/*																							  */
/* Battle Effects																			   */
/************************************************************************************************/
	bool m_bBattleEffect;
/************************************************************************************************/
/* RevolutionDCM				 Battle Effects END											 */
/************************************************************************************************/
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAttachableInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAttachableInfo :
	public CvInfoBase,
	public CvScalableInfo
{
public:

	CvAttachableInfo();
	virtual ~CvAttachableInfo();

	DllExport const TCHAR* getPath() const { return m_szPath; }
	void setPath(const TCHAR* szVal) { m_szPath = szVal; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvAttachableInfo* pClassInfo, CvXMLLoadUtility* pXML);

private:
	CvString m_szPath;
	float m_fUpdateRate;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCameraInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCameraInfo :
	public CvInfoBase
{
public:

	CvCameraInfo() {}
	virtual ~CvCameraInfo() {}

	const TCHAR* getPath() const { return m_szPath; }
	void setPath(const TCHAR* szVal) { m_szPath = szVal; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvCameraInfo* pClassInfo, CvXMLLoadUtility* pXML);

private:
	CvString m_szPath;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitArtStyleTypeInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitArtStyleTypeInfo : public CvInfoBase
{
public:

	CvUnitArtStyleTypeInfo();
	virtual ~CvUnitArtStyleTypeInfo();

	const TCHAR* getEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);

/************************************************************************************************/
/* Afforess					  Start		 03/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	const TCHAR* getClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvUnitArtStyleTypeInfo* pClassInfo, CvXMLLoadUtility* pXML);

protected:

	struct ArtDefneTag
	{
		int iMeshIndex;
		int iUnitType;
		CvString szTag;
	};
	typedef std::vector<ArtDefneTag> ArtDefineArray;
/************************************************************************************************/
/* Afforess					  Start		 03/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	ArtDefineArray m_azEarlyArtDefineTags;
	ArtDefineArray m_azClassicalArtDefineTags;
	ArtDefineArray m_azMiddleArtDefineTags;
	ArtDefineArray m_azRennArtDefineTags;
	ArtDefineArray m_azIndustrialArtDefineTags;
	ArtDefineArray m_azLateArtDefineTags;
	ArtDefineArray m_azFutureArtDefineTags;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMainMenuInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMainMenuInfo : public CvInfoBase
{
public:

	CvMainMenuInfo();
	virtual ~CvMainMenuInfo();

	DllExport std::string getScene() const;
	DllExport std::string getSceneNoShader() const;
	DllExport std::string getSoundtrack() const;
	DllExport std::string getLoading() const;
	DllExport std::string getLoadingSlideshow() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvMainMenuInfo* pClassInfo, CvXMLLoadUtility* pXML);

protected:
	std::string m_szScene;
	std::string m_szSceneNoShader;
	std::string m_szSoundtrack;
	std::string m_szLoading;
	std::string m_szLoadingSlideshow;
};
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 10/24/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
// Python Modular Loading
class CvPythonModulesInfo : public CvInfoBase
{
public:
	CvPythonModulesInfo();
	virtual ~CvPythonModulesInfo();

	bool isMainInterfaceScreen();
	bool isCivicScreen();
	bool isCorporationScreen();
	bool isDomesticAdvisor();
	bool isEspionageAdvisor();
	bool isForeignAdvisor();
	bool isMilitaryAdvisor();
	bool isVictoryScreen();
	int getScreen();

	bool read(CvXMLLoadUtility* pXML);

protected:

	bool m_bMainInterfaceScreen;
	bool m_bCivicScreen;
	bool m_bCorporationScreen;
	bool m_bDomesticAdvisor;
	bool m_bEspionageAdvisor;
	bool m_bForeignAdvisor;
	bool m_bMilitaryAdvisor;
	bool m_bVictoryScreen;
	int m_iScreen;

};

// MLF loading
class CvModLoadControlInfo : public CvInfoBase
{
public:

	CvModLoadControlInfo();
	virtual ~CvModLoadControlInfo();

	bool isLoad(int i);
	void setLoad(int i, bool bLoad = true);
	std::string getModuleFolder(int i);
	std::string getParentFolder();
	int getNumModules();
	int getDirDepth();
	bool read(CvXMLLoadUtility* pXML, CvString szDirDepth, int iDirDepth);

protected:
	bool* m_bLoad;
	int m_iNumModules;
	int m_iDirDepth;
	std::string* m_paszModuleFolder;
	std::string m_paszParentFolder;
};
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 END												  */
/************************************************************************************************/
#endif
