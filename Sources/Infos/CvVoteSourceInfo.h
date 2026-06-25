#pragma once

#ifndef CV_VOTE_SOURCE_INFO_H
#define CV_VOTE_SOURCE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVoteSourceInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteSourceInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvVoteSourceInfo();
	virtual ~CvVoteSourceInfo();

	int getVoteInterval() const;
	int getFreeSpecialist() const;
	int getCivic() const;
	const CvWString getPopupText() const;
	const CvWString getSecretaryGeneralText() const;

	std::wstring pyGetSecretaryGeneralText() const { return getSecretaryGeneralText(); }

	int getReligionYield(int i) const;
	int getReligionCommerce(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvVoteSourceInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iVoteInterval;
	int m_iFreeSpecialist;
	int m_iCivic;

	int* m_aiReligionYields;
	int* m_aiReligionCommerces;

	CvString m_szPopupText;
	CvString m_szSecretaryGeneralText;
};

#endif // CV_VOTE_SOURCE_INFO_H
