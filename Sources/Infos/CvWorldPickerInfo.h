#pragma once

#ifndef CV_WORLD_PICKER_INFO_H
#define CV_WORLD_PICKER_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvWorldPickerInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWorldPickerInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvWorldPickerInfo();
	virtual ~CvWorldPickerInfo();

	DllExport const char* getMapName();
	DllExport const char* getModelFile();
	DllExport int getNumSizes();
	DllExport float getSize(int index);
	DllExport int getNumClimates();
	DllExport const char* getClimatePath(int index);
	DllExport int getNumWaterLevelDecals();
	DllExport const char* getWaterLevelDecalPath(int index);
	DllExport int getNumWaterLevelGloss();
	DllExport const char* getWaterLevelGlossPath(int index);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvWorldPickerInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szMapName;
	CvString m_szModelFile;
	std::vector<float> m_aSizes;
	std::vector<CvString> m_aClimates;
	std::vector<CvString> m_aWaterLevelDecals;
	std::vector<CvString> m_aWaterLevelGloss;
};

#endif // CV_WORLD_PICKER_INFO_H
