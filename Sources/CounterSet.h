#pragma once
// CounterSet.h
#ifndef COUNTER_SET_H
#define COUNTER_SET_H

class CounterSet
{
	unsigned short iNext;
	unsigned short iSize;
	unsigned short* iArrCounters;
	int* iArrCurrentIDs;

	void assign(const int ID, const unsigned short iValue);

	unsigned short safeCoercion(const int iValue);

  public:
	CounterSet();
	CounterSet(const int iNumCounters);
	~CounterSet();
	void init(const int iNumCounters);
	void setCount(const int ID, const int iValue);
	unsigned short getCount(const int ID);
};

#endif