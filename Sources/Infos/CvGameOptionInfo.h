#pragma once

#ifndef CV_GAME_OPTION_INFO_H
#define CV_GAME_OPTION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameOptionInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:
	CvGameOptionInfo();
	virtual ~CvGameOptionInfo();

	DllExport bool getDefault() const { return m_bDefault; }
	DllExport bool getVisible() const { return m_bVisible; }

	//TB Tags
	const std::vector<GameOptionTypes>& getEnforcesGameOptionOnTypes() const  { return m_aEnforcesGameOptionOnTypes; }
	const std::vector<GameOptionTypes>& getEnforcesGameOptionOffTypes() const { return m_aEnforcesGameOptionOffTypes; }

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvGameOptionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
	bool m_bVisible;
	//TB Tags
	std::vector<GameOptionTypes> m_aEnforcesGameOptionOnTypes;
	std::vector<GameOptionTypes> m_aEnforcesGameOptionOffTypes;
};

#endif // CV_GAME_OPTION_INFO_H
