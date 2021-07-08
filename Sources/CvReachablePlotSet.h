#pragma once

#ifndef CvReachablePlotSet_h__
#define CvReachablePlotSet_h__

//#include <queue>

//	Activity ids against which enumerated plot info can be stored
#define	ACTIVITY_ID_ANY_ATTACK	0
#define	ACTIVITY_ID_EXPLORE		1
#define	ACTIVITY_ID_PILLAGE		2
#define	NUM_ACTIVITY_IDS		3

typedef struct CvReachablePlotInfo
{
	int	iStepDistance;
	int iOutsideOwnedDistance;
	int	iOpaqueInfo[NUM_ACTIVITY_IDS];
} CvReachablePlotInfo;

class CvPlot;
class CvSelectionGroup;

class CvReachablePlotSet
{
public:
	class const_iterator
	{
	friend CvReachablePlotSet;
	protected:
		const_iterator(const CvReachablePlotSet* parent, stdext::hash_map<CvPlot*,CvReachablePlotInfo>::const_iterator& itr);

	public:
		const_iterator& operator++();

		bool operator==(const const_iterator& other) const;

		bool operator!=(const const_iterator& other) const;

		const_iterator& operator=(const const_iterator& other);

		CvPlot*	plot() const;

		int stepDistance() const;
		int outsideBorderDistance() const;
		int getOpaqueInfo(int iActivityId) const;
		void setOpaqueInfo(int iActivityId, int iValue);

	private:
		stdext::hash_map<CvPlot*,CvReachablePlotInfo>::const_iterator m_itr;
		const CvReachablePlotSet* m_parent;
	};

	CvReachablePlotSet(const CvSelectionGroup * group, int iFlags, int iRange = -1, bool bCachable = true, int iOutsideOwnedRange = -1);

	~CvReachablePlotSet();

	const_iterator begin() const;

	const_iterator end() const;

	const_iterator find(CvPlot* plot) const;

	void Populate(int iRange);

	static void ClearCache();
private:

	static bool canMoveBetweenWithFlags(const CvSelectionGroup* group, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags);

	void enumerateReachablePlotsInternal(int iRange, int iDepth, std::vector< std::pair<CvPlot*, int> >& prevRing);

	int getRange() const;
	void setRange(int iRange);
	static CvReachablePlotSet* findCachedPlotSet(const CvSelectionGroup* pGroup, int iFlags, int iOutsideOwnedRange);

protected:
	stdext::hash_map<CvPlot*, CvReachablePlotInfo>* m_reachablePlots;
	const CvSelectionGroup*	m_group;
	int					m_iFlags;
	int					m_iRange;
	int					m_iOutsideOwnedRange;
	CvReachablePlotSet* m_proxyTo;
	bool				m_bCachable;
};

#endif // CvReachablePlotSet_h__
