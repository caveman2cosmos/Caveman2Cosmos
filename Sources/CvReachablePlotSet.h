#pragma once
#include <queue>
#include "FProfiler.h"

//	Activity ids against which enumerated plot info can be stored
#define ACTIVITY_ID_ANY_ATTACK 0
#define ACTIVITY_ID_EXPLORE	   1
#define ACTIVITY_ID_PILLAGE	   2

#define NUM_ACTIVITY_IDS 3

typedef struct CvReachablePlotInfo
{
	int iStepDistance;
	int iOutsideOwnedDistance;
	int iOpaqueInfo[NUM_ACTIVITY_IDS];
} CvReachablePlotInfo;

class CvReachablePlotSet
{
  public:
	class const_iterator
	{
		friend CvReachablePlotSet;

	  protected:
		const_iterator(CvReachablePlotSet& parent, stdext::hash_map<CvPlot*, CvReachablePlotInfo>::const_iterator& itr);

	  public:
		const_iterator& operator++();

		bool operator==(const_iterator& other);

		bool operator!=(const_iterator& other);

		const_iterator& operator=(const_iterator& other);

		CvPlot* plot();

		int	 stepDistance();
		int	 outsideBorderDistance();
		int	 getOpaqueInfo(int iActivityId) const;
		void setOpaqueInfo(int iActivityId, int iValue);

	  private:
		stdext::hash_map<CvPlot*, CvReachablePlotInfo>::const_iterator m_itr;
		CvReachablePlotSet&											   m_parent;
	};

	CvReachablePlotSet(CvSelectionGroup* group, int iFlags, int iRange = -1, bool bCachable = true, int iOutsideOwnedRange = -1);

	~CvReachablePlotSet();

	CvReachablePlotSet::const_iterator begin();

	CvReachablePlotSet::const_iterator end();

	CvReachablePlotSet::const_iterator find(CvPlot* plot);

	void Populate(int iRange);

	static void ClearCache();

  private:
	static bool canMoveBetweenWithFlags(CvSelectionGroup* group, CvPlot* pFromPlot, CvPlot* pToPlot, int iFlags);

	void enumerateReachablePlotsInternal(int iRange, int iDepth, std::vector<std::pair<CvPlot*, int> >& prevRing);

	int						   getRange() const;
	void					   setRange(int iRange);
	static CvReachablePlotSet* findCachedPlotSet(CvSelectionGroup* pGroup, int iFlags, int iOutsideOwnedRange);

  protected:
	stdext::hash_map<CvPlot*, CvReachablePlotInfo>* m_reachablePlots;
	CvSelectionGroup*								m_group;
	int												m_iFlags;
	int												m_iRange;
	int												m_iOutsideOwnedRange;
	CvReachablePlotSet*								m_proxyTo;
	bool											m_bCachable;
};
