#pragma once

#ifndef Cv_Diplomacy_Classes
#define Cv_Diplomacy_Classes


#include "CvGameCoreDLL.h"
#include "CvInfos.h"

class CvDiplomacyResponse
	: private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvDiplomacyResponse();
	virtual ~CvDiplomacyResponse();

	int getNumDiplomacyText() const;

	bool getCivilizationTypes(const int i) const;
	bool* getCivilizationTypes() const;

	bool getLeaderHeadTypes(const int i) const;
	bool* getLeaderHeadTypes() const;

	bool getAttitudeTypes(int i) const;
	bool* getAttitudeTypes() const;

	bool getDiplomacyPowerTypes(const int i) const;
	bool* getDiplomacyPowerTypes() const;

	const char* getDiplomacyText(int i) const;
	const CvString* getDiplomacyText() const;

	bool read(CvXMLLoadUtility* pXML);
	void UpdateDiplomacies(CvDiplomacyInfo* pDiplomacyInfo, int iIndex);

protected:
	int m_iNumDiplomacyText;
	bool* m_pbCivilizationTypes;
	bool* m_pbLeaderHeadTypes;
	bool* m_pbAttitudeTypes;
	bool* m_pbDiplomacyPowerTypes;
	CvString* m_paszDiplomacyText;
};


class CvDiplomacyInfo
	: public CvInfoBase
	, private bst::noncopyable
{

	friend class CvXMLLoadUtility;		// so it can access private vars to initialize the class
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvDiplomacyInfo();
	virtual ~CvDiplomacyInfo();

	void uninit();

	const CvDiplomacyResponse& getResponse(int iNum) const;
	int getNumResponses() const;

	bool getCivilizationTypes(int i, int j) const;
	bool getLeaderHeadTypes(int i, int j) const;
	bool getAttitudeTypes(int i, int j) const;
	bool getDiplomacyPowerTypes(int i, int j) const;

	int getNumDiplomacyText(int i) const;

	const char* getDiplomacyText(int i, int j) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvXMLLoadUtility* pXML);
	bool FindResponseIndex(const CvDiplomacyResponse* pNewResponse, int iCase, int* iIndex) const;

private:
	std::vector<CvDiplomacyResponse*> m_pResponses;
};

#endif