#pragma once

#ifndef VectorUtils_H
#define VectorUtils_H

class CvXMLLoadUtility;

namespace vector {

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
						if (value > -1  && !hasValue(value, aInfos))
							aInfos.push_back(value);

						if (!pXML.GetNextXmlVal(szTextVal))
							break;
					}
					//std::sort(aInfos.begin(), aInfos.end());

					pXML.MoveToXmlParent();
				}
			}
			pXML.MoveToXmlParent();
		}
	}

	template <class T>
	static void copyNonDefaults(std::vector<T>& target, const std::vector<T>& source)
	{
		foreach_(T it, source)
			if (it > -1 && !hasValue(it, target))
				target.push_back(it);

		std::sort(target.begin(), target.end());
	}

	template <class Container>
	inline void getCheckSum(uint32_t& iSum, const Container& kCont)
	{
		for (Container::const_iterator it = kCont.begin(); it != kCont.end(); ++it)
			CheckSum(iSum, *it);
	}

	template <class Container, typename ValueType>
	bool hasValue(const ValueType value, const Container& container)
	{
		foreach_(const ValueType element, container)
			if (element == value)
				return true;
		return false;
	}

	//template <class Container>
	//const python::list makePythonList(const Container& source)
	//{
	//	python::list list = python::list();
	//	for (Container::const_iterator it = source.begin(); it != source.end(); it++)
	//		list.append((int)*it);
	//	return list;
	//}
}

#endif // VectorUtils_H