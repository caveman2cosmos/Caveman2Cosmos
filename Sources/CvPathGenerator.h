#pragma once

#ifndef CvPathGenerator_h__
#define CvPathGenerator_h__

#include "FProfiler.h"

//#define	DYNAMIC_PATH_STRUCTURE_VALIDATION

class CvMap;
class CvPlot;
class CvPathNode;
class CvPathGenerator;
class CvSelectionGroup;

class CvPathGeneratorBase
{
public:
	virtual bool useAIPathingAlways() const { return m_useAIPathingAlways; };
	virtual const CvPlot* getTerminalPlot() const = 0;
protected:
	bool								m_useAIPathingAlways;
};

// Function prototype for Cost and Validity functions
typedef int(*HeuristicCost)(const CvSelectionGroup* pGroup, int iFromX, int iFromY, int iToX, int iToY, int& iLimitCost);
typedef int(*EdgeCost)(const CvPathGeneratorBase* generator, const CvSelectionGroup* pGroup, int iFromX, int iFromY, int iToX, int iToY, int iFlags, int& iMovementRemaining, int iPathTurns, int& iToNodeCost, bool bIsTerminalNode);
typedef bool(*EdgeValidity)(const CvSelectionGroup* pGroup, int iFromX, int iFromY, int iToX, int iToY, int iFlags, bool isTerminus, bool bAssertTerminatesMove, int iPathTurns, bool& bToNodeInvalidity);
typedef bool(*TerminusValidity)(const CvSelectionGroup* pGroup, int iToX, int iToY, int iFlags, bool& bRequiresWar);
typedef bool(*TurnEndValidityCheckRequired)(const CvSelectionGroup* pGroup, int iFlags);

class CvPath
{
friend CvPathGenerator;
public:
	class const_iterator
	{
	friend CvPath;
	protected:
		explicit const_iterator(CvPathNode* cursorNode);

	public:
		const_iterator& operator++();

		bool operator==(const const_iterator& other) const;

		bool operator!=(const const_iterator& other) const;

		CvPlot*	plot() const;
		int		turn() const;

	private:
		CvPathNode*	m_cursorNode;
	};

protected:
	CvPath();

	void Set(CvPathNode* startNode);

public:
	const const_iterator begin() const;
	const const_iterator end() const;

	int	length() const;
	CvPlot*	lastPlot() const;
	bool	containsEdge(const CvPlot* pFromPlot, const CvPlot* pToPlot) const;
	bool	containsNode(const CvPlot* pPlot) const;
	void	trimBefore(const CvPlot* pPlot);
	int		movementRemaining() const;
	int		cost() const;

private:
	CvPathNode* m_startNode;
	CvPathNode* m_endNode;
};

//	Helper class to manage allocation of tree nodes efficiently
typedef std::vector<CvPathNode> PoolBucket;

template <class T>
class CvAllocationPool
{
private:
	class AllocationType
	{
	public:
		AllocationType() {}

#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
		int	m_seq;
#endif
		T	m_data;
	};

public:
	CvAllocationPool()
	{
		m_nextBucketToAllocate = 0;
#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
		m_iSeq = 0;
#endif
		reset();
	}
	~CvAllocationPool()
	{
		PROFILE_EXTRA_FUNC();
		while(m_nextBucketToAllocate-- > 0)
		{
			SAFE_DELETE(m_pool[m_nextBucketToAllocate]);
		}
	}

	T*		allocateNode()
	{
		//PROFILE("CvAllocationPool.allocateNode");

		int iCurrentBucketCapacity;

		if ( m_nextBucketIndex == 0 )
		{
			iCurrentBucketCapacity = 0;
		}
		else
		{
			iCurrentBucketCapacity = m_pool[m_nextBucketIndex-1]->capacity();
		}

		if ( m_nextIndex >= iCurrentBucketCapacity )
		{
			if ( m_nextBucketToAllocate == m_nextBucketIndex )
			{

				std::vector<AllocationType>* newBucket = new std::vector<AllocationType>();

				newBucket->reserve(range(iCurrentBucketCapacity*2, 16, 1024));
				m_pool.push_back(newBucket);

				m_nextBucketToAllocate++;
			}

			m_nextIndex = 0;
			m_nextBucketIndex++;
		}

		AllocationType& allocated = (*m_pool[m_nextBucketIndex-1])[m_nextIndex++];

#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
		allocated.m_seq = m_iSeq;
#endif
		return &allocated.m_data;
	}

	void	reset()
	{
		m_nextIndex = 0;
		m_nextBucketIndex = 0;
#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
		m_iSeq++;
#endif

		//	No point freeing the allocated pools - we'll just need them again
		//	for another use
	}

#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
	bool	isAllocated(T* data) const
	{
		AllocationType*	alloc = (AllocationType*)(((int*)data) - 1);

		return (m_iSeq == alloc->m_seq);
	}
#endif

private:
	int											m_nextBucketIndex;
	int											m_nextIndex;
	int											m_nextBucketToAllocate;
#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
	int											m_iSeq;
#endif
	std::vector<std::vector<AllocationType>*>	m_pool;
};

typedef struct
{
	CvPathNode*	node;
	int			iQueuedCost;
} priorityQueueEntry;

//	Forward declarations of helper classes
class CvPathPlotInfoStore;
class CvPathGeneratorPlotInfo;

class CvNodeCostInfo
{
public:
	CvPathNode*					m_parentNode;
	CvPlot*						m_pAdjacentPlot;
	CvPathGeneratorPlotInfo*	m_pAdjacentPlotInfo;
	int							m_iEdgeCost;
	int							m_iNodeCost;
	int							m_iMovementRemaining;
	int							m_iDirection;
	bool						m_bValid;
};

typedef struct
{
	CvPathNode* root;
	CvSelectionGroup* pGroup;
	int iMaxTurns;
	int iFlags;
	const CvPlot* pTo;
	bool bTurnEndValidCheckNeeded;
} CvPathContext;

//	Forward declarations of helper classes
class CvPathPlotInfoStore;
class CvPathGeneratorPlotInfo;

class CvPathGenerator : public CvPathGeneratorBase
{
public:
	explicit CvPathGenerator(CvMap* pMap);
	virtual ~CvPathGenerator();

	void reset();

	void Initialize(HeuristicCost HeuristicFunc, EdgeCost CostFunc, EdgeValidity ValidFunc, TerminusValidity TerminusValidFunc, TurnEndValidityCheckRequired TurnEndValidCheckNeeded);
	bool generatePath(const CvPlot* pFrom, const CvPlot* pTo, CvSelectionGroup* pGroup, int iFlags, int iMaxTurns, int iOptimizationLimit = -1);
	bool newgeneratePath(const CvPlot* pFrom, const CvPlot* pTo, CvSelectionGroup* pGroup, int iFlags, int iMaxTurns, int iOptimizationLimit = -1);
	bool generatePathForHypotheticalUnit(const CvPlot* pFrom, const CvPlot* pTo, PlayerTypes ePlayer, UnitTypes eUnit, int iFlags, int iMaxTurns);
	bool haveRouteLength(const CvPlot* pTo, CvSelectionGroup* pGroup, int iFlags, int& iRouteLen);

	virtual const CvPlot* getTerminalPlot() const;

	const CvPath& getLastPath() const;
	void SelfTest();

private:
	class CvPathNodeComparer
	{
	public:
		CvPathNodeComparer()
		{
		}
		bool operator() (const priorityQueueEntry& lhs, const priorityQueueEntry& rhs) const;
	};

	CvPathNode*	allocatePathNode();
	bool groupMatches(const CvSelectionGroup* pGroup, int iFlags, uint32_t& iGroupMembershipChecksum);
	void AdjustChildTreeCosts(CvPathNode* node, int iAmount, bool bHasQueued);
	void OrphanChildTree(CvPathNode* node);
	void DeleteChildTree(CvPathNode* node, bool bIsDeletionRoot);
	void ValidatePlotInfo(CvPathGeneratorPlotInfo* pPlotInfo);
	bool isDescendantOfReplacedNode(CvPathNode* node) const;
	void LinkNode(CvPathNode* node, CvPathNode* parent);
	void RelinkNode(CvPathNode* node, CvPathNode* parent);
	bool isBetterPath(
		 CvPathNode*	existing,
		 CvPathNode*	from,
		 int			iEdgeCost,
		 int			iMovementRemaining) const;

#ifdef DYNAMIC_PATH_STRUCTURE_VALIDATION
	bool ValidateTreeInternal(CvPathNode* root, int& iValidationSeq, CvPathNode* unreferencedNode, CvPathNode* referencedNode, int& iQueuedCount);
	void ValidateTree(CvPathNode* root, CvPathNode* unreferencedNode, CvPathNode* referencedNode);
#endif

private:
	CvMap*								m_map;
	std::priority_queue<priorityQueueEntry,std::vector<priorityQueueEntry>,CvPathNodeComparer> m_priorityQueue;
	CvPathPlotInfoStore*				m_plotInfo;
	CvAllocationPool<CvPathNode>*		m_nodeAllocationPool;
	bool								m_bFoundRoute;
	int									m_iTerminalNodeCost;
	CvPathNode*							m_pBestTerminalNode;
	CvPathNode*							m_pReplacedNonTerminalNode;
	const CvPlot*						m_pTerminalPlot;
	CvPath								m_generatedPath;
	uint32_t							m_currentGroupMembershipChecksum;
	const CvPlot*						m_pFrom;
	int									m_iFlags;
	int									m_iTurn;
	int									m_iSeq;
	//	Callbacks
	HeuristicCost						m_HeuristicFunc;
	EdgeCost							m_CostFunc;
	EdgeValidity						m_ValidFunc;
	TerminusValidity					m_TerminusValidFunc;
	TurnEndValidityCheckRequired		m_TurnEndValidCheckNeeded;

public:
	int									m_nodesProcessed;
	int									m_nodesCosted;
};
//#endif

#endif // CvPathGenerator_h__