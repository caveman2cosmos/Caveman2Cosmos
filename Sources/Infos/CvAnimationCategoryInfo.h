#pragma once

#ifndef CV_ANIMATION_CATEGORY_INFO_H
#define CV_ANIMATION_CATEGORY_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAnimationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAnimationCategoryInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
	public:

		CvAnimationCategoryInfo();
		virtual ~CvAnimationCategoryInfo();

		DllExport int getCategoryBaseID( );
		DllExport int getCategoryDefaultTo( );

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(const CvAnimationCategoryInfo* pClassInfo);

	//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:

		CvAnimationCategoryDefinition	m_kCategory;		//!< The pair(base IDs, default categories) defining the animation categories
		CvString						m_szDefaultTo;		//!< Holds the default to parameter, until all categories are read
};

#endif // CV_ANIMATION_CATEGORY_INFO_H
