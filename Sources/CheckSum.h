#pragma once

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CheckSum()
//
//  PURPOSE :   Function to calculate a checksum
//
//------------------------------------------------------------------------------------------------------

#ifndef CHECKSUM_H
#define CHECKSUM_H

inline void CheckSum(unsigned int& iSum, unsigned int iData)
{
	iSum += iData;
	iSum = (iSum >> 1) | (iSum << (sizeof(iSum)*8 - 1));  // rotate
}

inline void CheckSum(unsigned int& iSum, int iData)
{
	CheckSum(iSum, (unsigned int) iData);
}

inline void CheckSum(unsigned int& iSum, bool bData)
{
	CheckSum(iSum, (unsigned int) bData);
}

inline void CheckSum(unsigned int& iSum, float fData)
{
	// cppcheck-suppress invalidPointerCast
	CheckSum(iSum, *(unsigned int *)&fData);
}

inline void CheckSum(unsigned int& iSum, const int* aiData, int iNum)
{
	if (aiData)
	{
		for (int i = 0; i < iNum; i++)
			CheckSum(iSum, (unsigned int)aiData[i]);
	}
}

inline void CheckSum(unsigned int& iSum, const bool* abData, int iNum)
{
	if (abData)
	{
		for (int i = 0; i < iNum; i++)
			CheckSum(iSum, (unsigned int)abData[i]);
	}
}

inline void CheckSum(unsigned int& iSum, char cData)
{
	CheckSum(iSum, (unsigned int) cData);
}

template<typename T1, typename T2>
inline void CheckSum(unsigned int& iSum, const std::pair<T1, T2>& p)
{
	CheckSum(iSum, p.first);
	CheckSum(iSum, p.second);
}

template<typename Cont>
inline void CheckSumC(unsigned int& iSum, const Cont& kCont)
{
	for (Cont::const_iterator it = kCont.begin(); it != kCont.end(); ++it)
	{
		CheckSum(iSum, *it);
	}
}

#define CheckSumI(A, B, C) CheckSum(A, C, B)

#endif