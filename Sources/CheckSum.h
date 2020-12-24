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


inline void CheckSum(uint32_t& iSum, char cData)
{
	CheckSum(iSum, (uint32_t) cData);
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

inline void CheckSum(uint32_t& iSum, int8_t data)
{
	CheckSum(iSum, static_cast<uint32_t>(data));
}

inline void CheckSum(uint32_t& iSum, uint8_t data)
{
	CheckSum(iSum, static_cast<uint32_t>(data));
}

inline void CheckSum(uint32_t& iSum, int16_t data)
{
	CheckSum(iSum, static_cast<uint32_t>(data));
}

inline void CheckSum(uint32_t& iSum, uint16_t data)
{
	CheckSum(iSum, static_cast<uint32_t>(data));
}

template <typename T>
inline void CheckSum(uint32_t& iSum, const T* aData, int iNum)
{
	if (aData)
	{
		for (int i = 0; i < iNum; i++)
			CheckSum(iSum, (uint32_t)aData[i]);
	}
}

template<typename T1, typename T2>
inline void CheckSum(uint32_t& iSum, const std::pair<T1, T2>& p)
{
	CheckSum(iSum, p.first);
	CheckSum(iSum, p.second);
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