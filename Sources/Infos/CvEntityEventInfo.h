#pragma once

#ifndef CV_ENTITY_EVENT_INFO_H
#define CV_ENTITY_EVENT_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEntityEventInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEntityEventInfo
	: public CvInfoBase
	, private bst::noncopyable
{
		//---------------------------PUBLIC INTERFACE---------------------------------
	public:

		CvEntityEventInfo();
		virtual ~CvEntityEventInfo();

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(const CvEntityEventInfo* pClassInfo);

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

#endif // CV_ENTITY_EVENT_INFO_H
