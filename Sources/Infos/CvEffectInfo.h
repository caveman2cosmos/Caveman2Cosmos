#pragma once

#ifndef CV_EFFECT_INFO_H
#define CV_EFFECT_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEffectInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEffectInfo
	: public CvInfoBase
	, public CvScalableInfo
	, private bst::noncopyable
{
public:

	CvEffectInfo();
	virtual ~CvEffectInfo();

	DllExport const char* getPath() const { return m_szPath; }
	void setPath(const char* szVal) { m_szPath = szVal; }
	float getUpdateRate( ) const { return m_fUpdateRate; };
	void setUpdateRate( float fUpdateRate ) { m_fUpdateRate = fUpdateRate; }
	bool isProjectile() const { return m_bProjectile; };
	float getProjectileSpeed() const { return m_fProjectileSpeed; };
	float getProjectileArc() const { return m_fProjectileArc; };
	bool isSticky() const { return m_bSticky; };

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvEffectInfo* pClassInfo);

private:
	CvString m_szPath;
	float m_fUpdateRate;
	bool m_bProjectile;
	bool m_bSticky;
	float m_fProjectileSpeed;
	float m_fProjectileArc;
};

#endif // CV_EFFECT_INFO_H
