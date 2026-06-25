#pragma once

#ifndef CV_UNIT_ART_STYLE_TYPE_INFO_H
#define CV_UNIT_ART_STYLE_TYPE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitArtStyleTypeInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitArtStyleTypeInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvUnitArtStyleTypeInfo();
	virtual ~CvUnitArtStyleTypeInfo();

	const char* getEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);

	const char* getClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvUnitArtStyleTypeInfo* pClassInfo);

protected:

	struct ArtDefneTag
	{
		int iMeshIndex;
		int iUnitType;
		CvString szTag;
	};
	typedef std::vector<ArtDefneTag> ArtDefineArray;

	ArtDefineArray m_azEarlyArtDefineTags;
	ArtDefineArray m_azClassicalArtDefineTags;
	ArtDefineArray m_azMiddleArtDefineTags;
	ArtDefineArray m_azRennArtDefineTags;
	ArtDefineArray m_azIndustrialArtDefineTags;
	ArtDefineArray m_azLateArtDefineTags;
	ArtDefineArray m_azFutureArtDefineTags;
};

#endif // CV_UNIT_ART_STYLE_TYPE_INFO_H
