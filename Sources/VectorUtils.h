#pragma once

#ifndef VectorUtils_H
#define VectorUtils_H

#include "algorithm2.h"

class CvXMLLoadUtility;
//class FDataStreamBase;

namespace vector
{
	using namespace algo::range;
/*
	void read(FDataStreamBase* pStream)
	{
		unsigned int iSize = 0;
		pStream->Read(&iSize);
		m_map.resize(iSize);
		for (unsigned int i = 0; i < iSize; i++)
		{
			pStream->Read(&(m_map[i].first));
		}
	}

	void write(FDataStreamBase* pStream)
	{
		pStream->Write(m_map.size());
		for (unsigned int i = 0; i < m_map.size(); i++)
		{
			pStream->Write(m_map[i].first);
		}
	}
*/
	template <class T>
	static void copyNonDefaults(std::vector<T>& target, const std::vector<T> source)
	{
		foreach_(T it, source)
			if (it > -1 && !contains(it, target))
				target.push_back(it);

		std::sort(target.begin(), target.end());
	}

	template <class T>
	void read(CvXMLLoadUtility& pXML, std::vector<T>& aInfos, const wchar_t* szRootTagName)
	{
		if (pXML.TryMoveToXmlFirstChild(szRootTagName))
		{
			const int iNumSibs = pXML.GetXmlChildrenNumber();
			if (0 < iNumSibs)
			{
				CvString szTextVal;
				if (pXML.GetChildXmlVal(szTextVal))
				{
					for (int j = 0; j < iNumSibs; j++)
					{
						//OutputDebugString(szTextVal.c_str());

						const T value = static_cast<T>(pXML.GetInfoClass(szTextVal));
						if (value > -1  && !contains(value, aInfos))
							aInfos.push_back(value);

						if (!pXML.GetNextXmlVal(szTextVal))
							break;
					}
					std::sort(aInfos.begin(), aInfos.end());

					pXML.MoveToXmlParent();
				}
			}
			pXML.MoveToXmlParent();
		}
	}

	template <class Cont>
	inline void getCheckSum(uint32_t& iSum, const Cont kCont)
	{
		for (Cont::const_iterator it = kCont.begin(); it != kCont.end(); ++it)
		{
			CheckSum(iSum, (uint32_t)*it);
		}
	}
}

#endif // VectorUtils_H