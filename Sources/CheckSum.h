//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CheckSum()
//
//  PURPOSE :   Function to calculate a checksum
//
//------------------------------------------------------------------------------------------------------

#ifndef CHECKSUM_H
#define CHECKSUM_H

inline void CheckSum(uint32_t& iSum, uint32_t iData)
{
	iSum += iData;
	iSum = (iSum >> 1) | (iSum << (sizeof(iSum)*8 - 1));  // rotate
}

inline void CheckSum(uint32_t& iSum, int iData)
{
	CheckSum(iSum, (uint32_t) iData);
}

inline void CheckSum(uint32_t& iSum, bool bData)
{
	CheckSum(iSum, (uint32_t) bData);
}

inline void CheckSum(uint32_t& iSum, float fData)
{
	// cppcheck-suppress invalidPointerCast
	CheckSum(iSum, *(uint32_t*)&fData);
}

inline void CheckSum(uint32_t& iSum, const int* aiData, int iNum)
{
	if (aiData)
	{
		for (int i = 0; i < iNum; i++)
			CheckSum(iSum, (uint32_t)aiData[i]);
	}
}

inline void CheckSum(uint32_t& iSum, const bool* abData, int iNum)
{
	if (abData)
	{
		for (int i = 0; i < iNum; i++)
			CheckSum(iSum, (uint32_t)abData[i]);
	}
}

inline void CheckSum(uint32_t& iSum, char cData)
{
	CheckSum(iSum, (uint32_t) cData);
}

template<typename T1, typename T2>
inline void CheckSum(uint32_t& iSum, const std::pair<T1, T2>& p)
{
	CheckSum(iSum, p.first);
	CheckSum(iSum, p.second);
}

template <typename T, size_t N>
inline void CheckSum(uint32_t& iSum, const bst::array<T, N>& kArray)
{
	foreach_(const T& element, kArray)
		CheckSum(iSum, element);
}

template<typename Cont>
inline void CheckSumC(uint32_t& iSum, const Cont& kCont)
{
	for (Cont::const_iterator it = kCont.begin(); it != kCont.end(); ++it)
	{
		CheckSum(iSum, *it);
	}
}

#define CheckSumI(A, B, C) CheckSum(A, C, B)

#endif