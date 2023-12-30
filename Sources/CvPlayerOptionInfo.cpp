#include "CvGameCoreDLL.h"
#include "CvPlayerOptionInfo.h"

CvPlayerOptionInfo::CvPlayerOptionInfo() :
m_bDefault(false)
{
}

CvPlayerOptionInfo::~CvPlayerOptionInfo()
{
}

bool CvPlayerOptionInfo::getDefault() const
{
	return m_bDefault;
}

bool CvPlayerOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}

void CvPlayerOptionInfo::copyNonDefaults(const CvPlayerOptionInfo* pClassInfo)
{
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}

void CvPlayerOptionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bDefault);
}