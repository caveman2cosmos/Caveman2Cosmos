#pragma once

//	$Revision: #2 $		$Author: mbreitkreutz $ 	$DateTime: 2005/06/13 13:35:55 $
//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FFreeListTrashArray.h
//
//  AUTHOR:  Soren Johnson
//
//  PURPOSE: A dynamic array with a free list that keeps track of its own memory...
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FFREELISTTRASHARRAY_H
#define		FFREELISTTRASHARRAY_H

#include	"FFreeListArrayBase.h"
#include	"FDataStreamBase.h"
#include	"CvTaggedSaveFormatWrapper.h"


#define FLTA_ID_SHIFT				(13)
#define FLTA_MAX_BUCKETS		(1 << FLTA_ID_SHIFT)
#define FLTA_INDEX_MASK			(FLTA_MAX_BUCKETS - 1)
#define FLTA_ID_MASK				(~(FLTA_INDEX_MASK))
#define FLTA_GROWTH_FACTOR	(2)

template <class T>
class FFreeListTrashArray : public FFreeListArrayBase<T>
{
public:

	FFreeListTrashArray();
	virtual ~FFreeListTrashArray();

	virtual void init(int iNumSlots = 8);
	virtual void uninit();
	virtual T* getAt(int iID) const;

	T* add();
	bool remove(T* pData);
	bool removeAt(int iID);
	virtual void removeAll();

	void load(T* pData);

	int getNumSlots() const
	{
		return m_iNumSlots;
	}

	int getLastIndex() const
	{
		return m_iLastIndex;
	}
	void setLastIndex(int iNewValue)
	{
		m_iLastIndex = iNewValue;
	}

	int getFreeListHead() const
	{
		return m_iFreeListHead;
	}
	void setFreeListHead(int iNewValue)
	{
		m_iFreeListHead = iNewValue;
	}

	int getFreeListCount() const
	{
		return m_iFreeListCount;
	}
	void setFreeListCount(int iNewValue)
	{
		m_iFreeListCount = iNewValue;
	}

	int getCorruptedAdjustment() const
	{
		return m_iCorruptedAdjustment;
	}
	void setCorruptedAdjustment(int iNewValue)
	{
		m_iCorruptedAdjustment = iNewValue;
	}

	int getCurrentID()
	{
		return m_iCurrentID;
	}

	void setCurrentID(int iNewValue)
	{
		assert((iNewValue & FLTA_INDEX_MASK) == 0);
		assert((iNewValue & FLTA_ID_MASK) != 0);
		m_iCurrentID = iNewValue;
	}

	int getNextFreeIndex(int iIndex)
	{
		if ((iIndex >= getNumSlots()) || (m_pArray == NULL))
		{
			assert(false);
			return FFreeList::INVALID_INDEX;
		}
		return m_pArray[iIndex].iNextFreeIndex;
	}
	void setNextFreeIndex(int iIndex, int iNewValue)
	{
		if ((iIndex >= getNumSlots()) || (m_pArray == NULL))
		{
			assert(false);
			return;
		}
		m_pArray[iIndex].iNextFreeIndex = iNewValue;
		m_pArray[iIndex].iLastUsed = m_iCurrentID;
	}

	void Read( FDataStreamBase* pStream );
	void Write( FDataStreamBase* pStream );

protected:

	struct FFreeListTrashArrayNode
	{
		int iNextFreeIndex;
		int iLastUsed;
		T* pData;
	};

	int m_iCurrentID;
	FFreeListTrashArrayNode* m_pArray;

	virtual void growArray();
};



// Public functions...

template <class T>
FFreeListTrashArray<T>::FFreeListTrashArray()
{
	m_iCurrentID = FLTA_MAX_BUCKETS;
	m_pArray = NULL;
}


template <class T>
FFreeListTrashArray<T>::~FFreeListTrashArray()
{
	uninit();
}


template <class T>
void FFreeListTrashArray<T>::init(int iNumSlots)
{
	assert(iNumSlots >= 0);

	// make sure it's binary...
	if ((iNumSlots > 0) && ((iNumSlots - 1) & iNumSlots) != 0)
	{
		// find high bit
		int iCount = 0;
		while (iNumSlots != 1)
		{
			iNumSlots >>= 1;
			iCount++;
		}
		iNumSlots = (1 << (iCount + 1));
	}

	assert(((iNumSlots - 1) & iNumSlots) == 0);
	assert((m_iNumSlots <= FLTA_MAX_BUCKETS) && "FFreeListTrashArray<T>::init() size too large");

	uninit();

	m_iNumSlots = iNumSlots;
	m_iLastIndex = FFreeList::INVALID_INDEX;
	m_iFreeListHead = FFreeList::INVALID_INDEX;
	m_iFreeListCount = 0;
	m_iCurrentID = FLTA_MAX_BUCKETS;

	if (m_iNumSlots > 0)
	{
		m_pArray = new FFreeListTrashArrayNode[m_iNumSlots];

		for (int iI = 0; iI < m_iNumSlots; iI++)
		{
			m_pArray[iI].iNextFreeIndex = FFreeList::INVALID_INDEX;
			m_pArray[iI].iLastUsed = FFreeList::INVALID_INDEX;
			m_pArray[iI].pData = NULL;
		}
	}
}


template <class T>
void FFreeListTrashArray<T>::uninit()
{
	if (m_pArray != NULL)
	{
		removeAll();

		SAFE_DELETE_ARRAY(m_pArray);
	}
}



template <class T>
T* FFreeListTrashArray<T>::add()
{
	int iIndex;

	if (m_pArray == NULL) 
	{
		init();
	}

	if ((m_iLastIndex == m_iNumSlots - 1) &&
		(m_iFreeListCount == 0))
	{
		if ((m_iNumSlots * FLTA_GROWTH_FACTOR) > FLTA_MAX_BUCKETS)
		{
			return NULL;
		}

		growArray();
	}

	if (m_iFreeListCount > 0)
	{
		int iRecount = 0;
		m_iFreeListCount--;

		int iLast = FFreeList::INVALID_INDEX;
		iIndex = m_iFreeListHead;

		//	Ttry to choose a free node that is not from the current major id space so as
		//	to minimize the rate we move through the global id space
		while( iIndex != FFreeList::INVALID_INDEX && m_pArray[iIndex].iLastUsed == m_iCurrentID )
		{
			iRecount++;
			iLast = iIndex;
			if (m_pArray[iIndex].iNextFreeIndex < FFreeList::INVALID_INDEX)
			{
				m_pArray[iIndex].iNextFreeIndex = FFreeList::INVALID_INDEX;
				int iCorrectionLoss = m_iFreeListCount - iRecount;
				m_iLastIndex -= iCorrectionLoss;
				m_iFreeListCount = iRecount;
			}
			iIndex = m_pArray[iIndex].iNextFreeIndex;
		}

		if ( iIndex == FFreeList::INVALID_INDEX )
		{
			//    Didn't find one - just take the free list head node
			if (m_iFreeListHead == FFreeList::INVALID_INDEX)
			{					
				m_iCorruptedAdjustment++;
				m_iLastIndex++;
				iIndex = m_iLastIndex;
			}
			else
			{
				iIndex = m_iFreeListHead;
				m_iFreeListHead = m_pArray[iIndex].iNextFreeIndex;
			}
		}
		else
		{
			//	Foudn one - take it out of the middle of the free list
			if ( iLast == FFreeList::INVALID_INDEX )
			{
				m_iFreeListHead = m_pArray[iIndex].iNextFreeIndex;
			}
			else
			{
				m_pArray[iLast].iNextFreeIndex = m_pArray[iIndex].iNextFreeIndex;
			}
		}
	}
	else
	{
		m_iLastIndex++;
		iIndex = m_iLastIndex;
	}

	MEMORY_TRACK_EXEMPT();

	m_pArray[iIndex].pData = new T;
	m_pArray[iIndex].iNextFreeIndex = FFreeList::INVALID_INDEX;

	if ( m_pArray[iIndex].iLastUsed == m_iCurrentID )
	{
		m_iCurrentID += FLTA_MAX_BUCKETS;
	}
	m_pArray[iIndex].pData->setID(m_iCurrentID + iIndex);
	m_pArray[iIndex].iLastUsed = m_iCurrentID;

	return m_pArray[iIndex].pData;
}


template <class T>
T* FFreeListTrashArray<T>::getAt(int iID) const
{
	int iIndex;

	if ((iID == FFreeList::INVALID_INDEX) || (m_pArray == NULL))
	{
		return NULL;
	}

	iIndex = (iID & FLTA_INDEX_MASK);

	assert(iIndex >= 0);

	if ((iIndex <= m_iLastIndex) && 
		(m_pArray[iIndex].pData != NULL))
	{
		if (((iID & FLTA_ID_MASK) == 0) || (m_pArray[iIndex].pData->getID() == iID))
		{
			return m_pArray[iIndex].pData;
		}
	}

	return NULL;
}


template <class T>
bool FFreeListTrashArray<T>::remove(T* pData)
{
	assert(m_pArray != NULL);

	if (pData != NULL)
	{
		for (int iI = 0; iI <= m_iLastIndex; iI++)
		{
			if (m_pArray[iI].pData == pData)
			{
				return removeAt(iI);
			}
		}
	}

	return false;
}


template <class T>
bool FFreeListTrashArray<T>::removeAt(int iID)
{
	if ((iID == FFreeList::INVALID_INDEX) || (m_pArray == NULL))
	{
		return false;
	}

	int iIndex = (iID & FLTA_INDEX_MASK);

	assert(iIndex >= 0);

	if ((iIndex <= m_iLastIndex) && 
		(m_pArray[iIndex].pData != NULL))
	{
		if (((iID & FLTA_ID_MASK) == 0) || (m_pArray[iIndex].pData->getID() == iID))
		{
			delete m_pArray[iIndex].pData;
			m_pArray[iIndex].pData = NULL;

			m_pArray[iIndex].iNextFreeIndex = m_iFreeListHead;
			m_iFreeListHead = iIndex;
			m_iFreeListCount++;

			return true;
		}
		else
		{
			assert(false);
		}
	}

	return false;
}


template <class T>
void FFreeListTrashArray<T>::removeAll()
{
	if (m_pArray == NULL)
	{
		return;
	}

	m_iLastIndex = FFreeList::INVALID_INDEX;
	m_iFreeListHead = FFreeList::INVALID_INDEX;
	m_iFreeListCount = 0;

	for (int iI = 0; iI < m_iNumSlots; iI++)
	{
		m_pArray[iI].iNextFreeIndex = FFreeList::INVALID_INDEX;
		if (m_pArray[iI].pData != NULL)
		{
			delete m_pArray[iI].pData;
		}
		m_pArray[iI].pData = NULL;
	}
}


template <class T>
void FFreeListTrashArray<T>::load(T* pData)
{
	assert(pData != NULL);
	//assert((pData->getID() & FLTA_ID_MASK) < m_iCurrentID);
	assert(m_pArray != NULL);

	int iIndex = (pData->getID() & FLTA_INDEX_MASK);

	assert(iIndex < FLTA_MAX_BUCKETS);
	assert(iIndex <= m_iLastIndex);
	assert(m_pArray[iIndex].pData == NULL);
	assert(m_pArray[iIndex].iNextFreeIndex == FFreeList::INVALID_INDEX);

	m_pArray[iIndex].pData = pData;
	m_pArray[iIndex].iLastUsed = (pData->getID() & FLTA_ID_MASK);
}

// Protected functions...

template <class T>
void FFreeListTrashArray<T>::growArray()
{
	MEMORY_TRACK_EXEMPT();

	assert(m_pArray != NULL);

	FFreeListTrashArrayNode* pOldArray = m_pArray;
	int iOldNumSlots = m_iNumSlots;

	m_iNumSlots *= FLTA_GROWTH_FACTOR;
	assert((m_iNumSlots <= FLTA_MAX_BUCKETS) && "FFreeListTrashArray<T>::growArray() size too large");
	m_pArray = new FFreeListTrashArrayNode[m_iNumSlots];

	for (int iI = 0; iI < m_iNumSlots; iI++)
	{
		if (iI < iOldNumSlots)
		{
			m_pArray[iI] = pOldArray[iI];
		}
		else
		{
			m_pArray[iI].iNextFreeIndex = FFreeList::INVALID_INDEX;
			m_pArray[iI].iLastUsed = FFreeList::INVALID_INDEX;
			m_pArray[iI].pData = NULL;
		}
	}

	delete [] pOldArray;
}

//
// use when list contains non-streamable types
//
template < class T >
inline void FFreeListTrashArray< T >::Read( FDataStreamBase* pStream )
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	int iTemp;
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "numSlots" );
	init( iTemp );
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "lastIndex" );
	setLastIndex( iTemp );
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "freeListHead" );
	setFreeListHead( iTemp );
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "freeListCount" );
	setFreeListCount( iTemp );
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "currentID" );
	setCurrentID( iTemp );

	for (int i = 0; i < getNumSlots(); i++ )
	{
		WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iTemp, "nextFreeIndex" );
		setNextFreeIndex( i, iTemp );
	}

	int iCount;
	WRAPPER_READ_DECORATED(wrapper, "FFreeListTrashArray", &iCount, "count" );

	for (int  i = 0; i < iCount; i++ )
	{
		T* pData = new T;
		WRAPPER_READ_ARRAY_DECORATED(wrapper, "FFreeListTrashArray", sizeof ( T ), ( byte* )pData, "pData" );
		load( pData );
	}

	WRAPPER_READ_OBJECT_END(wrapper);
}

template < class T >
inline void FFreeListTrashArray< T >::Write( FDataStreamBase* pStream )
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE_DECORATED(wrapper, "FFreeListTrashArray",  getNumSlots(), "numSlots" );
	WRAPPER_WRITE_DECORATED(wrapper, "FFreeListTrashArray",  getLastIndex(), "lastIndex" );
	WRAPPER_WRITE_DECORATED(wrapper, "FFreeListTrashArray",  getFreeListHead(), "freeListHead" );
	WRAPPER_WRITE_DECORATED(wrapper, "FFreeListTrashArray",  getFreeListCount(), "freeListCount" );
	WRAPPER_WRITE_DECORATED(wrapper, "FFreeListTrashArray",  getCurrentID(), "currentID" );

	for ( int i = 0; i < getNumSlots(); i++ )
	{
		WRAPPER_WRITE_DECORATED(wrapper, "FFreeListTrashArray",  getNextFreeIndex( i ), "nextFreeIndex" );
	}

	WRAPPER_WRITE_DECORATED(wrapper, "FFreeListTrashArray",  getCount(), "count" );

	for ( int i = 0; i < getIndexAfterLast(); i++ )
	{
		if ( getAt( i ) )
		{
			WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "FFreeListTrashArray",  sizeof ( T ), ( byte* )getAt( i ), "pData" );
		}
	}

	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray",  flist.getCorruptedAdjustment(), "flistCorruptedAdjustment" );

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

//-------------------------------
// Serialization helper templates:
//-------------------------------

//
// use when list contains streamable types
//
template < class T >
inline void ReadStreamableFFreeListTrashArray( FFreeListTrashArray< T >& flist, FDataStreamBase* pStream )
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	int iTemp;
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistNumSlots" );
	flist.init( iTemp );
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistLastIndex" );
	flist.setLastIndex( iTemp );
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistFreeListHead" );
	flist.setFreeListHead( iTemp );
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistFreeListCount" );
	flist.setFreeListCount( iTemp );
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistCurrentId" );
	flist.setCurrentID( iTemp );

	for ( int i = 0; i < flist.getNumSlots(); i++ )
	{
		WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistNextFree" );
		flist.setNextFreeIndex( i, iTemp );
	}

	int iCount;
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iCount, "flistCount" );

	for ( int i = 0; i < iCount; i++ )
	{
		T* pData = new T;
		pData->read( pStream );
		flist.load( pData );
	}

	iTemp = 0;
	WRAPPER_READ_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray", &iTemp, "flistCorruptedAdjustment" );
	flist.setCorruptedAdjustment( iTemp );

	WRAPPER_READ_OBJECT_END(wrapper);
}

template < class T >
inline void WriteStreamableFFreeListTrashArray( FFreeListTrashArray< T >& flist, FDataStreamBase* pStream )
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray",  flist.getNumSlots(), "flistNumSlots" );
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray",  flist.getLastIndex(), "flistLastIndex" );
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray",  flist.getFreeListHead(), "flistFreeListHead" );
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray",  flist.getFreeListCount(), "flistFreeListCount" );
	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray",  flist.getCurrentID(), "flistCurrentId" );

	for ( int i = 0; i < flist.getNumSlots(); i++ )
	{
		WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray",  flist.getNextFreeIndex( i ), "flistNextFree" );
	}

	WRAPPER_WRITE_DECORATED(wrapper, "WriteStreamableFFreeListTrashArray",  flist.getCount(), "flistCount" );

	for ( int i = 0; i < flist.getIndexAfterLast(); i++ )
	{
		if ( flist[ i ] )
		{
			flist[ i ]->write( pStream );
		}
	}

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

#endif	//FFREELISTTRASHARRAY_H
