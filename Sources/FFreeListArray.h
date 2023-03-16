#pragma once

#ifndef FFreeListArray_h__
#define FFreeListArray_h__

#include "FProfiler.h"

#include "FDataStreamBase.h"
#include "FFreeListArrayBase.h"

template <class T>
class FFreeListArray
{
public:

	FFreeListArray();
	~FFreeListArray();

	void init(int iNumSlots = 8);
	void uninit();
	T* getAt(int iIndex) const;
	T* operator[](int iIndex) const { return getAt(iIndex); }

	// start at the beginning of the list and return the first item or NULL when done
	T* beginIter(int* pIterIdx) const;

	// iterate from the current position and return the next item found or NULL when done
	T* nextIter(int* pIterIdx) const;

	// start at the end of the list and return the last item or NULL when done
	T* endIter(int* pIterIdx) const;

	// iterate from the current position and return the prev item found or NULL when done
	T* prevIter(int* pIterIdx) const;

	// Returns the iIndex after the last iIndex in the array containing an element
	int getIndexAfterLast() const { return m_iLastIndex + 1; }

	// Returns the number of elements in the array (NOTE: this is a non-packed array, so this value is NOT the last iIndex in the array...)
	int getCount()	const { return m_iLastIndex - m_iFreeListCount + 1 - m_iCorruptedAdjustment; }

	void insert(T data);
	void insertAt(T data, int iIndex);
	void insertFirst(T data);

	int getIndex(const T data) const;

	bool remove(const T data);
	bool removeAt(int iIndex);
	void removeAll();

	void Read( FDataStreamBase* pStream );
	void Write( FDataStreamBase* pStream );


private:
	int m_iFreeListHead;
	int m_iFreeListCount;
	int m_iLastIndex;
	int m_iNumSlots;
	int m_iCorruptedAdjustment;

	struct DArrayNode
	{
		int iNextFreeIndex;
		T		data;
	};

	DArrayNode* m_pArray;

	void growArray();
};



// Public Functions...

template <class T>
FFreeListArray<T>::FFreeListArray()
	: m_iFreeListHead(FFreeList::FREE_LIST_INDEX)
	, m_iFreeListCount(0)
	, m_iLastIndex(FFreeList::INVALID_INDEX)
	, m_iNumSlots(0)
	, m_iCorruptedAdjustment(0)
	, m_pArray(NULL)
{}


template <class T>
FFreeListArray<T>::~FFreeListArray()
{
	uninit();
}


template <class T>
void FFreeListArray<T>::init(int iNumSlots)
{
	uninit();

	m_iFreeListHead = FFreeList::FREE_LIST_INDEX;
	m_iFreeListCount = 0;
	m_iLastIndex = FFreeList::INVALID_INDEX;
	m_iNumSlots = iNumSlots;

	if (m_iNumSlots > 0)
	{
		m_pArray = new DArrayNode[m_iNumSlots];

		for (int iI = 0; iI < m_iNumSlots; iI++)
		{
			m_pArray[iI].iNextFreeIndex = FFreeList::INVALID_INDEX;
		}
	}
}


template <class T>
void FFreeListArray<T>::uninit()
{
	if (m_pArray != NULL)
	{
		removeAll();

		SAFE_DELETE_ARRAY(m_pArray);
	}
}

//
// iteration functions
//

// start at the beginning of the list and return the first item or NULL when done
template <class T>
T* FFreeListArray<T>::beginIter(int* pIterIdx) const
{
	*pIterIdx = 0;
	return nextIter(pIterIdx);
}

// iterate from the current position and return the next item found or NULL when done
template <class T>
T* FFreeListArray<T>::nextIter(int* pIterIdx) const
{
	for (; (*pIterIdx) < getIndexAfterLast(); (*pIterIdx)++)
	{
		T* pObj = getAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)++;	// prime for next call
			return pObj;
		}
	}
	return NULL;
}

// start at the end of the list and return the last item or NULL when done
template <class T>
T* FFreeListArray<T>::endIter(int* pIterIdx) const
{
	*pIterIdx = getIndexAfterLast() - 1;
	return prevIter(pIterIdx);
}

// iterate from the current position and return the prev item found or NULL when done
template <class T>
T* FFreeListArray<T>::prevIter(int* pIterIdx) const
{
	for (; (*pIterIdx) >= 0; (*pIterIdx)--)
	{
		T* pObj = getAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)--;	// prime for next call
			return pObj;
		}
	}
	return NULL;
}

template <class T>
void FFreeListArray<T>::insert(T data)
{

	if (m_pArray == NULL)
	{
		init();
	}

	if ((m_iLastIndex == m_iNumSlots - 1) &&
		(m_iFreeListCount == 0))
	{
		growArray();
	}

	int iIndex;
	if (m_iFreeListCount > 0)
	{
		iIndex = m_iFreeListHead;
		m_iFreeListHead = m_pArray[m_iFreeListHead].iNextFreeIndex;
		m_iFreeListCount--;
	}
	else
	{
		m_iLastIndex++;
		iIndex = m_iLastIndex;
	}

	m_pArray[iIndex].data = data;
	m_pArray[iIndex].iNextFreeIndex = FFreeList::INVALID_INDEX;
}


template <class T>
void FFreeListArray<T>::insertAt(T data, int iIndex)
{
	if (m_pArray == NULL)
	{
		init();
	}

	if (iIndex <= m_iLastIndex)
	{
		if (m_pArray[iIndex].iNextFreeIndex == FFreeList::INVALID_INDEX)
		{
			m_pArray[iIndex].data = data;
			return;
		}
	}

	while (iIndex > m_iNumSlots - 1)
	{
		growArray();
	}

	if (iIndex > m_iLastIndex)
	{
		while (iIndex != m_iLastIndex + 1)
		{
			m_iLastIndex++;
			m_pArray[m_iLastIndex].iNextFreeIndex = m_iFreeListHead;
			m_iFreeListHead = m_iLastIndex;
			m_iFreeListCount++;
		}

		m_iLastIndex++;
	}

	m_pArray[iIndex].data = data;

	if (m_iFreeListHead != FFreeList::FREE_LIST_INDEX)
	{
		if (iIndex == m_iFreeListHead)
		{
			m_iFreeListHead = m_pArray[iIndex].iNextFreeIndex;
			m_iFreeListCount--;
		}
		else
		{
			int iTempIndex = m_iFreeListHead;
			while (iTempIndex != FFreeList::FREE_LIST_INDEX)
			{
				FAssert(iTempIndex != FFreeList::INVALID_INDEX);
				if (m_pArray[iTempIndex].iNextFreeIndex == iIndex)
				{
					m_pArray[iTempIndex].iNextFreeIndex = m_pArray[iIndex].iNextFreeIndex;
					m_iFreeListCount--;
					break;
				}
				iTempIndex = m_pArray[iTempIndex].iNextFreeIndex;
			}
		}
	}

	m_pArray[iIndex].iNextFreeIndex = FFreeList::INVALID_INDEX;
}


template <class T>
void FFreeListArray<T>::insertFirst(T data)
{
	if (m_pArray == NULL)
	{
		init();
	}

	if ((m_iLastIndex == m_iNumSlots - 1) &&
		(m_iFreeListCount == 0))
	{
		growArray();
	}

	for (int iI = 0; iI <= m_iLastIndex; iI++)
	{
		if (m_pArray[iI].iNextFreeIndex != FFreeList::INVALID_INDEX)
		{
			insertAt(data, iI);
			return;
		}
	}

	insert(data);
}


template <class T>
T* FFreeListArray<T>::getAt(int iIndex) const
{
	if ((m_pArray == NULL) || (iIndex == FFreeList::INVALID_INDEX))
	{
		return NULL;
	}

	if ((iIndex >= 0) && (iIndex <= m_iLastIndex))
	{
		if (m_pArray[iIndex].iNextFreeIndex == FFreeList::INVALID_INDEX)
		{
			return &(m_pArray[iIndex].data);
		}
	}

	return NULL;
}


template <class T>
int FFreeListArray<T>::getIndex(const T data) const
{
	if (m_pArray == NULL)
	{
		return FFreeList::INVALID_INDEX;
	}

	for (int iI = 0; iI <= m_iLastIndex; iI++)
	{
		if (m_pArray[iI].iNextFreeIndex == FFreeList::INVALID_INDEX)
		{
			if (m_pArray[iI].data == data)
			{
				return iI;
			}
		}
	}

	return FFreeList::INVALID_INDEX;
}


template <class T>
bool FFreeListArray<T>::remove(const T data)
{
	FAssert(m_pArray != NULL);

	for (int iI = 0; iI <= m_iLastIndex; iI++)
	{
		if (m_pArray[iI].iNextFreeIndex == FFreeList::INVALID_INDEX)
		{
			if (m_pArray[iI].data == data)
			{
				return removeAt(iI);
			}
		}
	}

	return false;
}


template <class T>
bool FFreeListArray<T>::removeAt(int iIndex)
{
	FAssert(m_pArray != NULL);

	if ((iIndex >= 0) && (iIndex <= m_iLastIndex))
	{
		if (m_pArray[iIndex].iNextFreeIndex == FFreeList::INVALID_INDEX)
		{
			m_pArray[iIndex].iNextFreeIndex = m_iFreeListHead;
			m_iFreeListHead = iIndex;
			m_iFreeListCount++;

			return true;
		}
	}

	return false;
}


template <class T>
void FFreeListArray<T>::removeAll()
{
	if (m_pArray == NULL)
	{
		return;
	}

	m_iFreeListHead = FFreeList::FREE_LIST_INDEX;
	m_iFreeListCount = 0;
	m_iLastIndex = FFreeList::INVALID_INDEX;

	for (int iI = 0; iI < m_iNumSlots; iI++)
	{
		m_pArray[iI].iNextFreeIndex = FFreeList::INVALID_INDEX;
	}
}

// Protected functions...

template <class T>
void FFreeListArray<T>::growArray()
{
	DArrayNode* pOldArray = m_pArray;
	int iOldNumSlots = m_iNumSlots;

	m_iNumSlots *= 2;
	m_pArray = new DArrayNode[m_iNumSlots];

	for (int iI = 0; iI < m_iNumSlots; iI++)
	{
		if (iI < iOldNumSlots)
		{
			m_pArray[iI] = pOldArray[iI];
		}
		else
		{
			m_pArray[iI].iNextFreeIndex = FFreeList::INVALID_INDEX;
		}
	}

	delete [] pOldArray;
}

//
// use when list contains non-streamable types
//
template < class T >
inline void FFreeListArray< T >::Read( FDataStreamBase* pStream )
{
	int iCount = 0;
	pStream->Read( &iCount );
	init( iCount );

	if ( iCount )
	{
		for ( int i = 0; i < iCount; i++ )
		{
			T* pData = new T;
			pStream->Read( sizeof ( T ), ( byte* )pData );
			insert( pData );
		}
	}
}

template < class T >
inline void FFreeListArray< T >::Write( FDataStreamBase* pStream )
{
	int iCount = getCount();
	pStream->Write( iCount );

	for ( int i = 0; i < getIndexAfterLast(); i++ )
	{
		if ( getAt( i ) )
		{
			pStream->Write( sizeof ( T ), ( byte* )getAt( i ) );
		}
	}
}

//-------------------------------
// Serialization helper templates:
//-------------------------------

//
// use when list contains streamable types
//
template < class T >
inline void ReadStreamableFFreeListArray( FFreeListArray< T >& flist, FDataStreamBase* pStream )
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	pStream->Read( &iCount );
	flist.init( iCount );

	if ( iCount )
	{
		for ( int i = 0; i < iCount; i++ )
		{
			T* pData = new T;
			pData->read( pStream );
			flist.insert( pData );
		}
	}
}

//
// use when list contains streamable types
//
template < class T >
inline void WriteStreamableFFreeListArray( FFreeListArray< T >& flist, FDataStreamBase* pStream )
{
	PROFILE_EXTRA_FUNC();
	int iCount = flist.getCount();
	pStream->Write( iCount );

	for ( int i = 0; i < flist.getIndexAfterLast(); i++ )
	{
		if ( flist[ i ] )
		{
			flist[ i ]->write( pStream );
		}
	}
}

#endif // FFreeListArray_h__
