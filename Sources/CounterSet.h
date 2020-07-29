#pragma once
// Toffer - Simple set of counters
#ifndef COUNTER_SET_H
#define COUNTER_SET_H

class CounterSet
{
  private:
	int iNext;
	int iSize;
	int* iArrCounters;
	int* iArrCurrentIDs;

	void assign(const int ID, const int iValue);

  public:
	CounterSet();
	CounterSet(const int iNumCounters);
	~CounterSet();
	void init(int iNumCounters);
	int getCount(const int ID);
	void setCount(const int ID, const int iValue);
};

#endif