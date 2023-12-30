#include "CvGameCoreDLL.h"
#include "CvDiplomacyClasses.h"

#include "CvXMLLoadUtilityModTools.h"


//======================================================================================================
//					CvDiplomacyResponse
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvDiplomacyResponse()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyResponse::CvDiplomacyResponse() :
m_iNumDiplomacyText(0),
m_pbCivilizationTypes(NULL),
m_pbLeaderHeadTypes(NULL),
m_pbAttitudeTypes(NULL),
m_pbDiplomacyPowerTypes(NULL),
m_paszDiplomacyText(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvDiplomacyResponse()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyResponse::~CvDiplomacyResponse()
{
	SAFE_DELETE_ARRAY(m_pbCivilizationTypes);
	SAFE_DELETE_ARRAY(m_pbLeaderHeadTypes);
	SAFE_DELETE_ARRAY(m_pbAttitudeTypes);
	SAFE_DELETE_ARRAY(m_pbDiplomacyPowerTypes);
	SAFE_DELETE_ARRAY(m_paszDiplomacyText);
}

int CvDiplomacyResponse::getNumDiplomacyText() const
{
	return m_iNumDiplomacyText;
}

bool CvDiplomacyResponse::getCivilizationTypes(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivilizationInfos(), i);
	return m_pbCivilizationTypes ? m_pbCivilizationTypes[i] : false;
}

bool* CvDiplomacyResponse::getCivilizationTypes() const
{
	return m_pbCivilizationTypes;
}

bool CvDiplomacyResponse::getLeaderHeadTypes(const int i) const
{
	FASSERT_BOUNDS(0, GC.getNumLeaderHeadInfos(), i);
	return m_pbLeaderHeadTypes ? m_pbLeaderHeadTypes[i] : false;
}

bool* CvDiplomacyResponse::getLeaderHeadTypes() const
{
	return m_pbLeaderHeadTypes;
}

bool CvDiplomacyResponse::getAttitudeTypes(int i) const
{
	FASSERT_BOUNDS(0, NUM_ATTITUDE_TYPES, i);
	return m_pbAttitudeTypes ? m_pbAttitudeTypes[i] : false;
}

bool* CvDiplomacyResponse::getAttitudeTypes() const
{
	return m_pbAttitudeTypes;
}

bool CvDiplomacyResponse::getDiplomacyPowerTypes(const int i) const
{
	FASSERT_BOUNDS(0, NUM_DIPLOMACYPOWER_TYPES, i);
	return m_pbDiplomacyPowerTypes ? m_pbDiplomacyPowerTypes[i] : false;
}

bool* CvDiplomacyResponse::getDiplomacyPowerTypes() const
{
	return m_pbDiplomacyPowerTypes;
}

const char* CvDiplomacyResponse::getDiplomacyText(int i) const
{
	return m_paszDiplomacyText[i];
}

const CvString* CvDiplomacyResponse::getDiplomacyText() const
{
	return m_paszDiplomacyText;
}

bool CvDiplomacyResponse::read(CvXMLLoadUtility* pXML)
{
	pXML->SetVariableListTagPair(&m_pbCivilizationTypes, L"Civilizations", GC.getNumCivilizationInfos());
	// Leaders
	pXML->SetVariableListTagPair(&m_pbLeaderHeadTypes, L"Leaders", GC.getNumLeaderHeadInfos());
	// AttitudeTypes
	pXML->SetVariableListTagPair(&m_pbAttitudeTypes, L"Attitudes", NUM_ATTITUDE_TYPES);
	// PowerTypes
	pXML->SetVariableListTagPair(&m_pbDiplomacyPowerTypes, L"DiplomacyPowers", NUM_DIPLOMACYPOWER_TYPES);
	// DiplomacyText
	if (pXML->TryMoveToXmlFirstChild(L"DiplomacyText"))
	{
		pXML->SetStringList(&m_paszDiplomacyText, &m_iNumDiplomacyText);
		pXML->MoveToXmlParent();
	}

	return true;
}
void CvDiplomacyResponse::UpdateDiplomacies(CvDiplomacyInfo* pDiplomacyInfo, int iIndex)
{
	PROFILE_EXTRA_FUNC();
	const bool bDefault = false;

	// We use the String append mechanism from WOC default = assume the modder added his strings
	// purposly, so those are on the first place, the strings previously are appended after them
	CvString* m_paszNewNames = new CvString[pDiplomacyInfo->getNumDiplomacyText(iIndex)];
	for ( int i = 0; i < pDiplomacyInfo->getNumDiplomacyText(iIndex); i++)
	{
		m_paszNewNames[i] = pDiplomacyInfo->getDiplomacyText(iIndex, i);
	}

	CvXMLLoadUtilityModTools::StringArrayExtend(&m_paszDiplomacyText, &m_iNumDiplomacyText, &m_paszNewNames, pDiplomacyInfo->getNumDiplomacyText(iIndex));
	SAFE_DELETE_ARRAY(m_paszNewNames);

	// if anything is true, we don't overwrite(assuming the modder did set it true purposly
	for ( int i = 0; i < GC.getNumLeaderHeadInfos(); ++i)
	{
		if ((m_pbLeaderHeadTypes == NULL || m_pbLeaderHeadTypes[i] == bDefault) && pDiplomacyInfo->getLeaderHeadTypes(iIndex, i) != bDefault)
		{
			if ( NULL == m_pbLeaderHeadTypes )
			{
				CvXMLLoadUtility::InitList(&m_pbLeaderHeadTypes,GC.getNumLeaderHeadInfos(),bDefault);
			}
			m_pbLeaderHeadTypes[i] = pDiplomacyInfo->getLeaderHeadTypes(iIndex, i);
		}
	}
	for ( int i = 0; i < NUM_ATTITUDE_TYPES; ++i)
	{
		if ((m_pbAttitudeTypes == NULL || m_pbAttitudeTypes[i] == bDefault) && pDiplomacyInfo->getAttitudeTypes(iIndex, i) != bDefault)
		{
			if ( NULL == m_pbAttitudeTypes )
			{
				CvXMLLoadUtility::InitList(&m_pbAttitudeTypes,NUM_ATTITUDE_TYPES,bDefault);
			}
			m_pbAttitudeTypes[i] = pDiplomacyInfo->getAttitudeTypes(iIndex, i);
		}
	}
	for ( int i = 0; i < NUM_DIPLOMACYPOWER_TYPES; ++i)
	{
		if ((m_pbDiplomacyPowerTypes == NULL || m_pbDiplomacyPowerTypes[i] == bDefault) && pDiplomacyInfo->getDiplomacyPowerTypes(iIndex, i) != bDefault)
		{
			if ( NULL == m_pbDiplomacyPowerTypes )
			{
				CvXMLLoadUtility::InitList(&m_pbDiplomacyPowerTypes,NUM_DIPLOMACYPOWER_TYPES,bDefault);
			}
			m_pbDiplomacyPowerTypes[i] = pDiplomacyInfo->getDiplomacyPowerTypes(iIndex, i);
		}
	}
}

//======================================================================================================
//					CvDiplomacyInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvDiplomacyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyInfo::CvDiplomacyInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvDiplomacyInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyInfo::~CvDiplomacyInfo()
{
	uninit();
}

// note - Response member vars allocated by CvXmlLoadUtility
void CvDiplomacyInfo::uninit()
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvDiplomacyResponse* it, m_pResponses)
	{
		SAFE_DELETE(it);
	}
	m_pResponses.clear();
}

const CvDiplomacyResponse& CvDiplomacyInfo::getResponse(int iNum) const
{
	return *(m_pResponses[iNum]);
}

int CvDiplomacyInfo::getNumResponses() const
{
	return m_pResponses.size();
}

bool CvDiplomacyInfo::getCivilizationTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i)
	FASSERT_BOUNDS(0, GC.getNumCivilizationInfos(), j)
	return m_pResponses[i]->getCivilizationTypes(j);
}

bool CvDiplomacyInfo::getLeaderHeadTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i)
	FASSERT_BOUNDS(0, GC.getNumLeaderHeadInfos(), j)
	return m_pResponses[i]->getLeaderHeadTypes(j);
}

bool CvDiplomacyInfo::getAttitudeTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i)
	FASSERT_BOUNDS(0, NUM_ATTITUDE_TYPES, j)
	return m_pResponses[i]->getAttitudeTypes(j);
}

bool CvDiplomacyInfo::getDiplomacyPowerTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i)
	FASSERT_BOUNDS(0, NUM_DIPLOMACYPOWER_TYPES, j)
	return m_pResponses[i]->getDiplomacyPowerTypes(j);
}

int CvDiplomacyInfo::getNumDiplomacyText(int i) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i)
	return m_pResponses[i]->getNumDiplomacyText();
}

const char* CvDiplomacyInfo::getDiplomacyText(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i)
	FASSERT_BOUNDS(0, getNumDiplomacyText(i), j)
	return m_pResponses[i]->getDiplomacyText(j);
}

bool CvDiplomacyInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	int i;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	uninit();
	if ( pXML->TryMoveToXmlFirstChild(L"Responses") )
	{
		const int iNewResponses = pXML->GetXmlChildrenNumber(L"Response");

		if(pXML->TryMoveToXmlFirstChild())
		{
			for (i = 0; i < iNewResponses; i++)
			{
				CvDiplomacyResponse* pNewResponse = new CvDiplomacyResponse;
				pNewResponse->read(pXML);
				m_pResponses.push_back(pNewResponse);

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}
			pXML->MoveToXmlParent();
		}
		else
		{
			FErrorMsg("Diplomacy XML invalid");
			return false;
		}
		pXML->MoveToXmlParent();
	}
	else
	{
		FErrorMsg("Diplomacy XML invalid");
		return false;
	}

	return true;
}
void CvDiplomacyInfo::copyNonDefaults(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	// We have 6 possibilities in civ what this might be
	// 1) Text
	// 2) Leadertype + Text
	// 3) Attitude + Text
	// 4) Power + Text
	// 5) Leadertype + Attitude + Text
	// 6) Leadertype + Power + Text
	// Civilizations aren't used thus far...No idea why firaxis added this to the XML
	int iCase = 0; //no Case

	// Read the New Responses
	if ( pXML->TryMoveToXmlFirstChild(L"Responses") )
	{
		const int iNewResponses = pXML->GetXmlChildrenNumber(L"Response");

		if(pXML->TryMoveToXmlFirstChild())
		{
			for (int i = 0; i < iNewResponses; i++)
			{
				CvDiplomacyResponse* pNewResponse = new CvDiplomacyResponse;
				pNewResponse->read(pXML);

				// Check if the new
				bool bLeaderHeadTypes = false;
				//			bool bLeaderHeadTypesExist = false;
				for (int j = 0; j < GC.getNumLeaderHeadInfos(); j++)
				{
					if (pNewResponse->getLeaderHeadTypes(j))
					{
						bLeaderHeadTypes = true;
						/*					for ( int jj = 0; jj < iResponses; ++jj)
											{
												if ( getLeaderHeadTypes(jj, j) )
												{
													bLeaderHeadTypesExist = true;
													break;
												}
											}*/
						break;
					}
				}
				bool bAttitudeTypes = false;
				//			bool bAttitudeTypesExist = false;
				for (int j = 0; j < NUM_ATTITUDE_TYPES; j++)
				{
					if (pNewResponse->getAttitudeTypes(j))
					{
						bAttitudeTypes = true;
						/*					for ( int jj = 0; jj < iResponses; ++jj)
											{
												if ( getAttitudeTypes(jj, j) )
												{
													bAttitudeTypesExist = true;
													break;
												}
											}*/
						break;
					}
				}
				bool bPowerTypes = false;
				//			bool bPowerTypesExist = false;
				for (int j = 0; j < NUM_DIPLOMACYPOWER_TYPES; j++)
				{
					if (pNewResponse->getDiplomacyPowerTypes(j))
					{
						bPowerTypes = true;
						/*					for ( int jj = 0; jj < iResponses; ++jj)
											{
												if ( getDiplomacyPowerTypes(jj, j) )
												{
													bPowerTypesExist = true;
													break;
												}
											}*/
						break;
					}
				}

				// Check which case we have
				//			if ( bLeaderHeadTypes && !bLeaderHeadTypesExist) FErrorMsg(L"Wrong XML format!");
				//			if ( bAttitudeTypes && !bAttitudeTypesExist) FErrorMsg(L"Wrong XML format!");
				//			if ( bPowerTypes && !bPowerTypesExist) FErrorMsg(L"Wrong XML format!");
				if (!(bLeaderHeadTypes || bAttitudeTypes || bPowerTypes)) iCase = 1;
				else if (bLeaderHeadTypes && !(bAttitudeTypes || bPowerTypes)) iCase = 2;
				else if (bAttitudeTypes && !(bLeaderHeadTypes || bPowerTypes)) iCase = 3;
				else if (bPowerTypes && !(bLeaderHeadTypes || bAttitudeTypes)) iCase = 4;
				else if (bLeaderHeadTypes && bAttitudeTypes && !bPowerTypes) iCase = 5;
				else if (bLeaderHeadTypes && bPowerTypes && !bAttitudeTypes) iCase = 6;
				else FErrorMsg("A new case with an added Diplomacy XML but already exists?");

				// Find ResponseIndex(if already exists)
				int iIndex = 0;
				const bool bIsIndex = FindResponseIndex(pNewResponse, iCase, &iIndex);

				if (bIsIndex)
				{
					// Check the Previous Values of this class and copy them to the Diplomacies Response
					pNewResponse->UpdateDiplomacies(this, iIndex);

					//Delete old vector info(only for this response)
					//DeleteResponseOnly(m_pResponses, getResponse(iIndex));
					m_pResponses.erase(m_pResponses.begin() + iIndex);
				}

				//apply new vector info
				m_pResponses.push_back(pNewResponse);

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}
			pXML->MoveToXmlParent();
		}
		else
		{
			FErrorMsg("Diplomacy XML invalid");
		}
		pXML->MoveToXmlParent();
	}
	else
	{
		FErrorMsg("Diplomacy XML invalid");
	}
}
bool CvDiplomacyInfo::FindResponseIndex(const CvDiplomacyResponse* pNewResponse, int iCase, int* iIndex) const
{
	PROFILE_EXTRA_FUNC();
	// We have 6 possibilities in civ what this might be
	// 1) Text
	// 2) Leadertype + Text
	// 3) Attitude + Text
	// 4) Power + Text
	// 5) Leadertype + Attitude + Text
	// 6) Leadertype + Power + Text

	if ( getNumResponses() == 1 )
	{
		iIndex = 0;
		return true;
	}

	// Text
	if ( iCase == 1 )
	{
		bool bOnlyText = true;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) ) bOnlyText = false;
			}
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) ) bOnlyText = false;
			}
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) ) bOnlyText = false;
			}
			if ( bOnlyText )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Leaderheads + Text
	if ( iCase == 2 )
	{
		bool bOnlyLeaderheads = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) && pNewResponse->getLeaderHeadTypes(iElement) ) bOnlyLeaderheads = true;
			}
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) ) bOnlyLeaderheads = false;
			}
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) ) bOnlyLeaderheads = false;
			}
			if ( bOnlyLeaderheads )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Attitude + Text
	if ( iCase == 3 )
	{
		bool bOnlyAttitudeTypes = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) && pNewResponse->getAttitudeTypes(iElement)) bOnlyAttitudeTypes = true;
			}
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) ) bOnlyAttitudeTypes = false;
			}
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) ) bOnlyAttitudeTypes = false;
			}
			if ( bOnlyAttitudeTypes )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Power + Text
	if ( iCase == 4 )
	{
		bool bOnlyDiplomacyPowerTypes = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) && pNewResponse->getDiplomacyPowerTypes(iElement) ) bOnlyDiplomacyPowerTypes = true;
			}
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) ) bOnlyDiplomacyPowerTypes = false;
			}
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) ) bOnlyDiplomacyPowerTypes = false;
			}

			if ( bOnlyDiplomacyPowerTypes )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Leadertype + Attitude + Text
	if ( iCase == 5 )
	{
		bool bLeaderAttitudeTypes = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) && pNewResponse->getLeaderHeadTypes(iElement) )
				{
					for ( int iiElement = 0; iiElement < NUM_ATTITUDE_TYPES; ++iiElement )
					{
						if ( getAttitudeTypes(i, iiElement) && pNewResponse->getAttitudeTypes(iiElement) ) bLeaderAttitudeTypes = true;
					}
				}
			}
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) ) bLeaderAttitudeTypes = false;
			}

			if ( bLeaderAttitudeTypes )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Leadertype + Power + Text
	if ( iCase == 6 )
	{
		bool bLeaderDiplomacyPowerTypes = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) && pNewResponse->getLeaderHeadTypes(iElement) )
				{
					for ( int iiElement = 0; iiElement < NUM_DIPLOMACYPOWER_TYPES; ++iiElement )
					{
						if ( getDiplomacyPowerTypes(i, iiElement) && pNewResponse->getDiplomacyPowerTypes(iiElement) ) bLeaderDiplomacyPowerTypes = true;
					}
				}
			}
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) ) bLeaderDiplomacyPowerTypes = false;
			}

			if ( bLeaderDiplomacyPowerTypes )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	return false;  //This means there was no index, hence it's a new one and needs to be added to the vector
}